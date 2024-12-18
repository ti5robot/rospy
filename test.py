#!/usr/bin/env python

#coding:utf-8

import time
import ctypes
import struct
from ctypes import *
import numpy as np

##  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/astra/controlcan
## sudo chmod -R 777 /dev
## python3 test.py
class VCI_INIT_CONFIG(Structure):
    _fields_ = [("AccCode",c_uint),
            ("AccMask",c_uint),
            ("Reserved",c_uint),
            ("Filter",c_ubyte),
            ("Timing0",c_ubyte),
            ("Timing1",c_ubyte),
            ("Mode",c_ubyte)
            ]

class VCI_CAN_OBJ(Structure):
    _fields_ = [("ID",c_uint),
            ("TimeStamp",c_uint),
            ("TimeFlag",c_ubyte),
            ("SendType",c_ubyte),
            ("RemoteFlag",c_ubyte),
            ("ExternFlag",c_ubyte),
            ("DataLen",c_ubyte),
            ("Data",c_ubyte*8),
            ("Reserved",c_ubyte*3)
            ]

class VCI_CAN_OBJ_ARRAY(Structure):
    _fields_ = [('SIZE',ctypes.c_uint16),('STRUCT_ARRAY',ctypes.POINTER(VCI_CAN_OBJ))]

    def __init__(self,num_of_structs):
        self.STRUCT_ARRAY = ctypes.cast((VCI_CAN_OBJ * num_of_structs)(),ctypes.POINTER(VCI_CAN_OBJ))
        self.SIZE = num_of_structs
        self.ADDR = self.STRUCT_ARRAY[0]

canDLL = None
VCI_USBCAN2 = 4
STATUS_OK = 1 

def init_can():
    global canDLL
    global VCI_USBCAN2
    global STATUS_OK
    can_dll_name = '/home/astra/controlcan/libcontrolcan.so'
    canDLL = cdll.LoadLibrary('/home/astra/controlcan/libcontrolcan.so')

    print(can_dll_name)
    ret = canDLL.VCI_OpenDevice(VCI_USBCAN2,0,0)
    if ret == STATUS_OK:
        print("VCI_OpenDevice okk")
    else:
        print("VCI_OpenDevice fail")

        ## init tongdao 0
    vci_initconfig = VCI_INIT_CONFIG(0x80000008,0xFFFFFFFF,0,1,0x00,0x14,0) ### 1000K
    ret = canDLL.VCI_InitCAN(VCI_USBCAN2,0,0,byref(vci_initconfig))

    if ret == STATUS_OK:
        print("VCI_InitCAN okk")
    else:
        print("VCI_InitCAN fail")

    ret = canDLL.VCI_StartCAN(VCI_USBCAN2,0,0)
    if ret == STATUS_OK:
        print("VCI_startCAN okk")
    else:
        print("VCI_StartCAN fail")
        canDLL.VCI_CloseDevice(VCI_USBCAN2,0)

    ## init tongdao 1
    ret =canDLL.VCI_InitCAN(VCI_USBCAN2,0,1,byref(vci_initconfig))
    if ret == STATUS_OK:
        print("VCI_InitCAN2 okk")
    else:
        print("VCI_InitCAN2 fail")
    ret = canDLL.VCI_StartCAN(VCI_USBCAN2,0,1)
    if ret == STATUS_OK:
        print("VCI_StartCAN2 okk")
    else:
        print("VCI_StartCAN2 fail")
        canDLL.VCI_CloseDevice(VCI_USBCAN2,0)



def convert_hex_array_to_decimal(hex_array):
    result=0
    for i in range(4):
        result=(result<<8) | hex_array[i]
    if result > 0x7FFFFFFF:
        result -= 0x100000000
    return result


def to_int_array(number,size):
    unsigned_number = number if number >= 0 else (1 << 32) + number

    res = []
    for i in range(size):
        res.append(unsigned_number & 0xFF)
        unsigned_number >>= 8
    return res


def send_simple_can_command(num_of_actuator,can_id_list,command):
    global canDLL
    global VCI_USBCAN2
    global STATUS_OK
    send = VCI_CAN_OBJ()
    send.SendType = 0
    send.RemoteFlag = 0
    send.ExternFlag = 0
    send.DataLen = 1
    res = []
    for i in range(0,num_of_actuator):
        send.ID = i+1
        send.Data[0] = command
        if canDLL.VCI_Transmit(VCI_USBCAN2,0,0,byref(send),1) == 1:
            rec = VCI_CAN_OBJ_ARRAY(3000)
            ind=0
            cnt = 5

            reclen=canDLL.VCI_Receive(VCI_USBCAN2,0,0,byref(rec.ADDR),3000,0)
            while reclen<=0 and cnt>0:
                if reclen<=0:
                    reclen=canDLL.VCI_Receive(VCI_USBCAN2,0,0,byref(rec.ADDR),3000,0)
                cnt=cnt-1
            if cnt==0:
                print("ops! ID %d failed after try 5 times!",send.ID)
            else:
                for j in range(reclen):
                    data_array = [rec.STRUCT_ARRAY[j].Data[4],rec.STRUCT_ARRAY[j].Data[3],rec.STRUCT_ARRAY[j].Data[2],rec.STRUCT_ARRAY[j].Data[1]]
                    data_list = [data_array[i] for i in range(len(data_array))]
                    decimal = self.convert_hex_array_to_decimal(data_list)
                    res.append(decimal)
                    print("ID: {}\tdata: {}".format(send.ID,decimal))
        else:
            break
    return res


def send_can_command(num_of_actuator,can_id_list,command_list,parameter_list):
    global canDLL
    global VCI_USBCAN2
    global STATUS_OK
    send = VCI_CAN_OBJ()
    
    send.SendType = 0
    send.RemoteFlag = 0
    send.ExternFlag = 0
    send.DataLen = 5
    
    for i in range(num_of_actuator):
        send.ID = i+1
        send.Data[0] = command_list[i]
        res = to_int_array(parameter_list[i],4)

        for ii in range (1,5):
            send.Data[ii] = res[ii-1]
            ret=0
        canDLL.VCI_Transmit(VCI_USBCAN2,0,0,byref(send),1)




def main():
    init_can()

    canidlist = [1]
    cmd_pos = [30]
    cmd_get_pos = [8]

    pos = [int(400000)]
    cnt = 10
    while cnt > 0:
        cnt -= 1

        if cnt % 2 == 1:
            pos[0] = int(400000)
        else:
            pos[0] = int(-400000)

        send_can_command(1,canidlist,cmd_pos,pos)

        time.sleep(1)


if __name__=="__main__":
    main()
