// serial_demo.cpp
// serial_demo.cpp
// serial_demo.cpp
// serial_demo.cpp
// serial_demo.cpp
// serial_demo.cpp
#include <ros/ros.h>
#include <serial/serial.h>
#include <iostream>

#include "std_msgs/String.h"

#include <sensor_msgs/JointState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <robotmove.h>
#include <iostream>

#include <cstdint>
#define MESSAGE_FREQ 100
#define MESSAGE_FREQ_END 60

double next_velocitie[6] = {0};

double ori_velocitie[6] = {0};
double cha_velocitie[6] = {0};
double now_velocitie[6] = {0};

double next_position[6] = {0};

double ori_position[6] = {0};
double cha_position[6] = {0};
int now_position[6] = {0};
char buffer[5120];

int r1[500] = {0};
int r2[500] = {0};
int r3[500] = {0};

int r4[500] = {0};
int r5[500] = {0};
int r6[500] = {0};

class Listener
{
private:
    char topic_message[2048] = {0};

public:
    // void callback(const sensor_msgs::JointState::ConstPtr &msg);
    void callback(const moveit_msgs::DisplayTrajectory::ConstPtr &msg);
    char *getMessageValue();
};

double findMax(double arr[], int size) {
    double max = abs(arr[0]); // 假设数组中的第一个元素是最大值

    for (int i = 1; i < size-2; i++) {
        if (abs(arr[i]) > abs(max)) {
            max = abs(arr[i]); // 如果找到比当前最大值更大的值，则更新最大值
        }
    }

    return max;
}

int32_t crc32_table[256] = {0, 1996959894, -301047508, -1727442502, 124634137, 1886057615, -379345611, -1637575261, 249268274, 2044508324, -522852066, -1747789432, 162941995, 2125561021, -407360249, -1866523247, 498536548, 1789927666, -205950648, -2067906082, 450548861, 1843258603, -187386543, -2083289657, 325883990, 1684777152, -43845254, -1973040660, 335633487, 1661365465, -99664541, -1928851979, 997073096, 1281953886, -715111964, -1570279054, 1006888145, 1258607687, -770865667, -1526024853, 901097722, 1119000684, -608450090, -1396901568, 853044451, 1172266101, -589951537, -1412350631, 651767980, 1373503546, -925412992, -1076862698, 565507253, 1454621731, -809855591, -1195530993, 671266974, 1594198024, -972236366, -1324619484, 795835527, 1483230225, -1050600021, -1234817731, 1994146192, 31158534, -1731059524, -271249366, 1907459465, 112637215, -1614814043, -390540237, 2013776290, 251722036, -1777751922, -519137256, 2137656763, 141376813, -1855689577, -429695999, 1802195444, 476864866, -2056965928, -228458418, 1812370925, 453092731, -2113342271, -183516073, 1706088902, 314042704, -1950435094, -54949764, 1658658271, 366619977, -1932296973, -69972891, 1303535960, 984961486, -1547960204, -725929758, 1256170817, 1037604311, -1529756563, -740887301, 1131014506, 879679996, -1385723834, -631195440, 1141124467, 855842277, -1442165665, -586318647, 1342533948, 654459306, -1106571248, -921952122, 1466479909, 544179635, -1184443383, -832445281, 1591671054, 702138776, -1328506846, -942167884, 1504918807, 783551873, -1212326853, -1061524307, -306674912, -1698712650, 62317068, 1957810842, -355121351, -1647151185, 81470997, 1943803523, -480048366, -1805370492, 225274430, 2053790376, -468791541, -1828061283, 167816743, 2097651377, -267414716, -2029476910, 503444072, 1762050814, -144550051, -2140837941, 426522225, 1852507879, -19653770, -1982649376, 282753626, 1742555852, -105259153, -1900089351, 397917763, 1622183637, -690576408, -1580100738, 953729732, 1340076626, -776247311, -1497606297, 1068828381, 1219638859, -670225446, -1358292148, 906185462, 1090812512, -547295293, -1469587627, 829329135, 1181335161, -882789492, -1134132454, 628085408, 1382605366, -871598187, -1156888829, 570562233, 1426400815, -977650754, -1296233688, 733239954, 1555261956, -1026031705, -1244606671, 752459403, 1541320221, -1687895376, -328994266, 1969922972, 40735498, -1677130071, -351390145, 1913087877, 83908371, -1782625662, -491226604, 2075208622, 213261112, -1831694693, -438977011, 2094854071, 198958881, -2032938284, -237706686, 1759359992, 534414190, -2118248755, -155638181, 1873836001, 414664567, -2012718362, -15766928, 1711684554, 285281116, -1889165569, -127750551, 1634467795, 376229701, -1609899400, -686959890, 1308918612, 956543938, -1486412191, -799009033, 1231636301, 1047427035, -1362007478, -640263460, 1088359270, 936918000, -1447252397, -558129467, 1202900863, 817233897, -1111625188, -893730166, 1404277552, 615818150, -1160759803, -841546093, 1423857449, 601450431, -1285129682, -1000256840, 1567103746, 711928724, -1274298825, -1022587231, 1510334235, 755167117};

int16_t table_s[] = {12336, 12592, 12848, 13104, 13360, 13616, 13872, 14128, 14384, 14640, 16688, 16944, 17200, 17456, 17712, 17968, 12337, 12593, 12849, 13105, 13361, 13617, 13873, 14129, 14385, 14641, 16689, 16945, 17201, 17457, 17713, 17969, 12338, 12594, 12850, 13106, 13362, 13618, 13874, 14130, 14386, 14642, 16690, 16946, 17202, 17458, 17714, 17970, 12339, 12595, 12851, 13107, 13363, 13619, 13875, 14131, 14387, 14643, 16691, 16947, 17203, 17459, 17715, 17971, 12340, 12596, 12852, 13108, 13364, 13620, 13876, 14132, 14388, 14644, 16692, 16948, 17204, 17460, 17716, 17972, 12341, 12597, 12853, 13109, 13365, 13621, 13877, 14133, 14389, 14645, 16693, 16949, 17205, 17461, 17717, 17973, 12342, 12598, 12854, 13110, 13366, 13622, 13878, 14134, 14390, 14646, 16694, 16950, 17206, 17462, 17718, 17974, 12343, 12599, 12855, 13111, 13367, 13623, 13879, 14135, 14391, 14647, 16695, 16951, 17207, 17463, 17719, 17975, 12344, 12600, 12856, 13112, 13368, 13624, 13880, 14136, 14392, 14648, 16696, 16952, 17208, 17464, 17720, 17976, 12345, 12601, 12857, 13113, 13369, 13625, 13881, 14137, 14393, 14649, 16697, 16953, 17209, 17465, 17721, 17977, 12353, 12609, 12865, 13121, 13377, 13633, 13889, 14145, 14401, 14657, 16705, 16961, 17217, 17473, 17729, 17985, 12354, 12610, 12866, 13122, 13378, 13634, 13890, 14146, 14402, 14658, 16706, 16962, 17218, 17474, 17730, 17986, 12355, 12611, 12867, 13123, 13379, 13635, 13891, 14147, 14403, 14659, 16707, 16963, 17219, 17475, 17731, 17987, 12356, 12612, 12868, 13124, 13380, 13636, 13892, 14148, 14404, 14660, 16708, 16964, 17220, 17476, 17732, 17988, 12357, 12613, 12869, 13125, 13381, 13637, 13893, 14149, 14405, 14661, 16709, 16965, 17221, 17477, 17733, 17989, 12358, 12614, 12870, 13126, 13382, 13638, 13894, 14150, 14406, 14662, 16710, 16966, 17222, 17478, 17734, 17990};

char *Listener::getMessageValue()
{
    return topic_message;
}

serial::Serial sp;
size_t MemToStrStream(char *dest, const char *sourse, uint32_t size, char head, char end)
{
    uint32_t i = 0;
    uint8_t *pindex = (uint8_t *)sourse;
    *dest = head;
    int16_t *p = (int16_t *)(dest + 1);
    for (i = 0; i < size; i++)
    {
        p[i] = table_s[pindex[i]];
    }
    p[i] = end;
    //    qDebug()<<"end:"<<dest[size*2+1]<<dest[size*2+2];
    //    printf("**%d**%d**,",dest[size*2+1],dest[size*2+2]);
    return size * 2 + 2;
}

uint32_t calcCRC32(uint32_t crc, uint8_t *string, uint32_t size)
{
    while (size--)
        crc = (crc >> 8) ^ (crc32_table[(crc ^ *string++) & 0xff]);
    return crc;
}

std::int32_t convertHexArrayToDecimal(const std::uint8_t hexArray[4])
{
    std::int32_t result = 0;

    for (int i = 0; i < 4; i++)
    {
        result = (result << 8) | hexArray[i];
    }

    if (result > 0x7FFFFFFF) // Check if the result is a negative number
    {
        result -= 0x100000000; // Perform two's complement to get the correct negative value
    }

    return result;
}

void bufferTransfor(const std::string &buffer)
{
    std::string::size_type position;
    position = buffer.find("\n");
    std::cout << "length:" << buffer.length() << std::endl;
    std::cout << "index:" << position << std::endl;
    std::cout << "text:" << buffer << std::endl;

    int index = buffer.find("\n");
    std::cout << index << std::endl;
    std::cout << position << std::endl;
    uint8_t mem[400] = {0}, memIndex = 0;
    if (index > 5)
    { // 1

        std::cout << buffer << std::endl;
        std::string frame = buffer.substr(1, index - 1);
        // while (true)
        //{
        if (frame.length() > 1)
        {
            std::string w1 = frame.substr(0, 2);
            frame.erase(0, 2);
            std::cout << frame << std::endl;
        }
        // else{break;}

        //}
        std::cout << "break";
        // mem[memIndex]=
        // std::cout<<frame<<std::endl;

    } // 1
}

void toIntArray(int number, int *res, int size)
{
    unsigned int unsignedNumber = static_cast<unsigned int>(number);

    for (int i = 0; i < size; ++i)
    {
        res[i] = unsignedNumber & 0xFF; // 取最低8位的值
        unsignedNumber >>= 8;           // 将数值右移8位
    }
}

int32_t sendSimpleCanCommand(uint8_t numOfActuator, uint8_t *canIdList, int Command)
{

    VCI_CAN_OBJ send[1];
    send[0].ID = 1;
    send[0].SendType = 0;
    send[0].RemoteFlag = 0;
    send[0].ExternFlag = 0;
    send[0].DataLen = 1;
    int i = 0;
    for (i = 0; i < 6; i++)
    {
        send[0].Data[0] = Command; // 位置环30
        pthread_t threadid;

        if (VCI_Transmit(VCI_USBCAN2, 0, 0, send, 1) == 1)
        {

            //printf("CAN1 TX ID:0x%08X", send[0].ID);
            if (send[0].ExternFlag == 0)
                //printf(" Standard ");
            if (send[0].ExternFlag == 1)
                //printf(" Extend   ");
            if (send[0].RemoteFlag == 0)
                //printf(" Data   ");
            if (send[0].RemoteFlag == 1)
                //printf(" Remote ");
            //printf("DLC:0x%02X", send[0].DataLen);
            //printf(" data:0x");

            for (int iii = 0; iii < send[0].DataLen; iii++)
            {
                //printf(" %02X", send[0].Data[iii]);
            }
           //printf("\n");
            // read data from can cache
            int count = 0; // 数据列表中，用来存储列表序号。
            int reclen = 0;
            VCI_CAN_OBJ rec[3000]; // 接收缓存，设为3000为佳。
            int i, j;
            int ind = 0;

            if ((reclen = VCI_Receive(VCI_USBCAN2, 0, ind, rec, 3000, 100)) > 0) // 调用接收函数，如果有数据，进行数据处理显示。
            {

                for (j = 0; j < reclen; j++)
                // for(j=0;j<2;j++)
                {

                    std::uint8_t hexArray[4] = {rec[j].Data[4], rec[j].Data[3], rec[j].Data[2], rec[j].Data[1]};
                    //std::cout << "ID: " << send[0].ID ;
                    std::int32_t decimal = convertHexArrayToDecimal(hexArray);


                    //std::cout << "Decimal: " << decimal << std::endl;

                }
            }

            send[0].ID += 1;
        }
        else
        {
            break;
        }
    }
}

int32_t sendCanCommand(uint8_t numOfActuator, uint8_t *canIdList, uint8_t *commandList, uint32_t *parameterList)
{
    int Command = commandList[0]; // 位置环
    VCI_CAN_OBJ send[1];
    send[0].ID = 1;
    send[0].SendType = 0;
    send[0].RemoteFlag = 0;
    send[0].ExternFlag = 0;
    send[0].DataLen = 5;
    int i = 0;
    for (i = 0; i < 6; i++)
    {
        send[0].Data[0] = Command; // 位置环30
        //std::cout << "ID:" << i << "   " << parameterList[i] << std::endl;
        int res[4];
        toIntArray(parameterList[i], res, 4);
        int i_back = 4;
        for (int iiii = 1; iiii < 5; iiii++)
        {
            send[0].Data[iiii] = res[iiii - 1];
        }

        int m_run0 = 1;
        pthread_t threadid;
        int ret;

        if (VCI_Transmit(VCI_USBCAN2, 0, 0, send, 1) == 1)
        {

            printf("CAN1 TX ID:0x%08X", send[0].ID);
            if (send[0].ExternFlag == 0)
                printf(" Standard ");
            if (send[0].ExternFlag == 1)
                printf(" Extend   ");
            if (send[0].RemoteFlag == 0)
                printf(" Data   ");
            if (send[0].RemoteFlag == 1)
                printf(" Remote ");
            printf("DLC:0x%02X", send[0].DataLen);
            printf(" data:0x");

            for (int iii = 0; iii < send[0].DataLen; iii++)
            {
                printf(" %02X", send[0].Data[iii]);
            }
            //printf("ORI:%02X", parameterList[i]);

            printf("\n");
            send[0].ID += 1;
        }
        else
        {
            break;
        }
    }
}

int32_t sendCommand(uint8_t numOfActuator, uint8_t *canIdList, uint8_t *commandList, uint32_t *parameterList)
{

    uint8_t buf[400], index = 0;
    buf[index++] = numOfActuator;
    if (numOfActuator == 0)
    {
        buf[index++] = canIdList[0];
        buf[index++] = commandList[0];
        buf[index] = parameterList[0];
        index += 4;
    }
    else
    {
        for (int i = 0; i < numOfActuator; i++)
        {
            buf[index++] = canIdList[i];
        }
        for (int i = 0; i < numOfActuator; i++)
        {
            buf[index++] = commandList[i];
        }
        for (int i = 0; i < numOfActuator; i++)
        {

            *(uint32_t *)(buf + index) = parameterList[i];
            index += 4;

        }
    }

    *(uint32_t *)(buf + index) = calcCRC32(0, buf, index);
    index += 4;
    char uartTxBuf[2000];
    uint32_t byteToWrite = MemToStrStream(uartTxBuf, (char *)buf, index, '<', '\n');
    size_t writeSize = 0;
    // std::cout<<sp.write(uartTxBuf) <<std::endl;
    if (sp.write(uartTxBuf) == 24)
    {
        // std::cout<<uartTxBuf<<std::endl;
    }
    if (sp.write(uartTxBuf) == 48)
    {
        // std::cout<<uartTxBuf<<std::endl;
    }

    // std::cout<<uartTxBuf<<std::endl;
    // bool e123 =sp.write(uartTxBuf);
}

void Listener::callback(const moveit_msgs::DisplayTrajectory::ConstPtr &msg)
{
    int n = msg->trajectory[0].joint_trajectory.points.size();
    double cn[6];
    int t[6];

    

    char str[5120];
    char str123[5120];
    float ww;
    float vv;
    float zs;
    float zs1;
    float zss = 1.0;
    std::string end_position = "";

    SCanInterface *canInterface = SCanInterface::getInstance();
        int tmp0=100;
        int tmp1=-100;
        uint32_t parameterlist222[10] = {tmp0,tmp0,tmp0,tmp0,tmp0,tmp0};
        uint32_t parameterlist2222[10] = {tmp1,tmp1,tmp1,tmp1,tmp1,tmp1};
            uint8_t canidlist222[10] = {1,2, 3,4,5,6}, cmd222[10] = {36,36,36,36,36, 36};
            sendCanCommand(6, canidlist222, cmd222, parameterlist222);
        usleep(100);
	    uint8_t cmd2222[10] = {37,37,37,37,37,37};
            sendCanCommand(6, canidlist222, cmd2222, parameterlist2222);
        usleep(100);

    //for (int i = 0; i < n - 1; i++)
    //{
    
    
        ori_position[0] = msg->trajectory[0].joint_trajectory.points[0].positions[0]*57.3;
        ori_position[1] = msg->trajectory[0].joint_trajectory.points[0].positions[1] *57.3;
        ori_position[2] = msg->trajectory[0].joint_trajectory.points[0].positions[2] *57.3;
        ori_position[3] = msg->trajectory[0].joint_trajectory.points[0].positions[3] *57.3;
        ori_position[4] = msg->trajectory[0].joint_trajectory.points[0].positions[4] *57.3;
        ori_position[5] = msg->trajectory[0].joint_trajectory.points[0].positions[5] *57.3;
        next_position[0] = msg->trajectory[0].joint_trajectory.points[n-1].positions[0] *57.3;
        next_position[1] = msg->trajectory[0].joint_trajectory.points[n-1].positions[1] *57.3;
        next_position[2] = msg->trajectory[0].joint_trajectory.points[n-1].positions[2] *57.3;
        next_position[3] = msg->trajectory[0].joint_trajectory.points[n-1].positions[3] *57.3;
        next_position[4] = msg->trajectory[0].joint_trajectory.points[n-1].positions[4] *57.3;
        next_position[5] = msg->trajectory[0].joint_trajectory.points[n-1].positions[5] *57.3;

        now_position[0] = msg->trajectory[0].joint_trajectory.points[n-1].positions[0] *57.3*101*65536/360;
        now_position[1] = -(msg->trajectory[0].joint_trajectory.points[n-1].positions[1]) *57.3*101*65536/360;
        now_position[2] = -(msg->trajectory[0].joint_trajectory.points[n-1].positions[2]) *57.3*101*65536/360;
        now_position[3] = msg->trajectory[0].joint_trajectory.points[n-1].positions[3] *57.3*101*65536/360;
        now_position[4] = -(msg->trajectory[0].joint_trajectory.points[n-1].positions[4]) *57.3*101*65536/360;
        now_position[5] = msg->trajectory[0].joint_trajectory.points[n-1].positions[5] *57.3*101*65536/360;
	std::cout<<now_position[0]<<"**"<<now_position[1]<<"**"<<now_position[2]<<"**"<<now_position[3]<<"**"<<now_position[4]<<std::endl;

        cn[0] = next_position[0] - ori_position[0];
        cn[1] = next_position[1] - ori_position[1];
        cn[2] = next_position[2] - ori_position[2];
        cn[3] = next_position[3] - ori_position[3];
        cn[4] = next_position[4] - ori_position[4];
        cn[5] = next_position[5] - ori_position[5];



	
	uint32_t parameterlists2[10]={0};

	    for(size_t i = 0; i < 6; ++i) {
        parameterlists2[i] = static_cast<uint32_t>(now_position[i]); // 进行类型转换
    }
	    /*
	parameterlists2[0]=now_position[0];
	parameterlists2[1]=now_position[1];
	parameterlists2[2]=now_position[2];
	parameterlists2[3]=now_position[3];
	parameterlists2[4]=now_position[4];
	parameterlists2[5]=now_position[5];
	*/
          //start motor go to target
            //uint32_t parameterlist2[10] = {now_position[0],now_position[1],now_position[2],now_position[3],now_position[4],now_position[5]};









	 //run 100ms with 200speed;

	//ready control max speed
        double maxVal = findMax(cn, 6); // 传入数组和数组的大小
	std::cout<<"~~~~~~"<<maxVal<<std::endl;
	double periodTime =maxVal/130;
	double status=1;
	if (periodTime<0.2){
		status=0.02;
	}

	//double periodTime =maxVal/170;
	std::cout<<"TTTTTTT"<<periodTime<<std::endl;
	int maxSpeed[6];
	maxSpeed[0]=abs((cn[0]/periodTime)*10100/360);
	maxSpeed[1]=abs((cn[1]/periodTime)*10100/360);
	maxSpeed[2]=abs((cn[2]/periodTime)*10100/360);
	maxSpeed[3]=abs((cn[3]/periodTime)*10100/360);
	maxSpeed[4]=abs((cn[4]/periodTime)*10100/360);
	maxSpeed[5]=abs((cn[5]/periodTime)*10100/360);

	std::cout<<"##"<<maxSpeed[0]<<"##"<<maxSpeed[1]<<"##"<<maxSpeed[2]<<"##"<<maxSpeed[3]<<"##"<<maxSpeed[4]<<std::endl;

        //get max speed split 
	    uint8_t canidlist2[10] = {1, 2, 3, 4, 5, 6}, cmd2[10] = {30, 30, 30, 30, 30, 30};
            sendCanCommand(6, canidlist2, cmd2, parameterlists2);

	    /*
	for (int j=5;j<105;j++){
        int tmp02=j*20;
        int tmp12=-j*20;
        uint32_t parameterlist222b[10] = {tmp02,tmp02,tmp02,tmp02,tmp02,tmp02};
        uint32_t parameterlist2222b[10] = {tmp12,tmp12,tmp12,tmp12,tmp12,tmp12};
            uint8_t canidlist222b[10] = {1,2, 3,4,5,6}, cmd222b[10] = {36,36,36,36,36, 36};
            sendCanCommand(6, canidlist222b, cmd222b, parameterlist222b);
        usleep(10);
	    uint8_t cmd2222b[10] = {37,37,37,37,37,37};
            sendCanCommand(6, canidlist222b, cmd2222b, parameterlist2222b);
        usleep(10);
	usleep(3000);
	}
	*/




	for (int j=10; j<105; j++) {
    double acceleration_ratio = (j - 5) / 100.0;  // 计算加速度比例，范围从 0 到 1
    int tmp02[6] = {0};
    tmp02[0] = acceleration_ratio * maxSpeed[0];
    tmp02[1] = acceleration_ratio * maxSpeed[1];
    tmp02[2] = acceleration_ratio * maxSpeed[2];
    tmp02[3] = acceleration_ratio * maxSpeed[3];
    tmp02[4] = acceleration_ratio * maxSpeed[4];
    tmp02[5] = acceleration_ratio * maxSpeed[5];

    int tmp12[6] = {-tmp02[0], -tmp02[1], -tmp02[2], -tmp02[3], -tmp02[4], -tmp02[5]};

    uint32_t parameterlist222b[10] = {tmp02[0], tmp02[1], tmp02[2], tmp02[3], tmp02[4], tmp02[5]};
    uint32_t parameterlist2222b[10] = {tmp12[0], tmp12[1], tmp12[2], tmp12[3], tmp12[4], tmp12[5]};

    uint8_t canidlist222b[10] = {1, 2, 3, 4, 5, 6}, cmd222b[10] = {36, 36, 36, 36, 36, 36};
    sendCanCommand(6, canidlist222b, cmd222b, parameterlist222b);

    usleep(10);

    uint8_t cmd2222b[10] = {37, 37, 37, 37, 37, 37};
    sendCanCommand(6, canidlist222b, cmd2222b, parameterlist2222b);

    usleep(10);

    usleep(5000 * status);
}

        //parameterlist2[10] = {0};
	
	//run maxspeed with periodTime-200000
	usleep(periodTime*1000000-550000*status);
	//
	//usleep(periodTime*1000000-500000);

	//cn unit is du
for (int j = 105; j > 5; j--) {
    double deceleration_ratio = (105 - j) / 100.0;  // 计算减速度比例，范围从 0 到 1
    int tmp02[6] = {0};
    tmp02[0] = j * maxSpeed[0] / 100;
    tmp02[1] = j * maxSpeed[1] / 100;
    tmp02[2] = j * maxSpeed[2] / 100;
    tmp02[3] = j * maxSpeed[3] / 100;
    tmp02[4] = j * maxSpeed[4] / 100;
    tmp02[5] = j * maxSpeed[5] / 100;

    int tmp12[6] = {-tmp02[0], -tmp02[1], -tmp02[2], -tmp02[3], -tmp02[4], -tmp02[5]};

    uint32_t parameterlist222b[10] = {tmp02[0], tmp02[1], tmp02[2], tmp02[3], tmp02[4], tmp02[5]};
    uint32_t parameterlist2222b[10] = {tmp12[0], tmp12[1], tmp12[2], tmp12[3], tmp12[4], tmp12[5]};

    uint8_t canidlist222b[10] = {1, 2, 3, 4, 5, 6}, cmd222b[10] = {36, 36, 36, 36, 36, 36};
    sendCanCommand(6, canidlist222b, cmd222b, parameterlist222b);

    usleep(10);

    uint8_t cmd2222b[10] = {37, 37, 37, 37, 37, 37};
    sendCanCommand(6, canidlist222b, cmd2222b, parameterlist2222b);

    usleep(10);

    usleep(5000 * status * deceleration_ratio);
}



	std::cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<std::endl;
	//
	//
	/*
        int tmp01=200;
        int tmp11=-200;
        uint32_t parameterlist222a[10] = {tmp01,tmp01,tmp01,tmp01,tmp01,tmp01};
        uint32_t parameterlist2222a[10] = {tmp11,tmp11,tmp11,tmp11,tmp11,tmp11};
            uint8_t canidlist222a[10] = {1,2, 3,4,5,6}, cmd222a[10] = {36,36,36,36,36, 36};
            sendCanCommand(6, canidlist222a, cmd222a, parameterlist222a);
        usleep(100);
	    uint8_t cmd2222a[10] = {37,37,37,37,37,37};
            sendCanCommand(6, canidlist222a, cmd2222a, parameterlist2222a);
        usleep(100);
	//
	usleep(200000);
	*/

	
	
	
}
bool init_can()
{
    VCI_BOARD_INFO pInfo; // 用来获取设备信息。
    int count = 0;        // 数据列表中，用来存储列表序号。
    VCI_BOARD_INFO pInfo1[50];
    int num = 0;
    fflush(stdin);
    fflush(stdout);
    printf(">>this is hello !\r\n"); // 指示程序已运行
    num = VCI_FindUsbDevice2(pInfo1);
    printf(">>USBCAN DEVICE NUM:");
    printf("%d", num);
    printf(" PCS");
    printf("\n");
    int nDeviceType = 4; /* USBCAN-2A或USBCAN-2C或CANalyst-II */
    int nDeviceInd = 0;  /* 第1个设备 */
    int nCANInd = 0;     /* 第1个通道 */
    DWORD dwRel;
    VCI_INIT_CONFIG vic;
    dwRel = VCI_OpenDevice(nDeviceType, nDeviceInd, 0);
    if (dwRel != 1)
    {
        std::cout << "open-fail:" << dwRel << std::endl;
        return FALSE;
    }
    else
    {
        std::cout << "open success:1";
    }
    vic.AccCode = 0x80000008;
    vic.AccMask = 0xFFFFFFFF;
    vic.Filter = 1;
    vic.Timing0 = 0x00;
    vic.Timing1 = 0x14;
    vic.Mode = 0;
    dwRel = VCI_InitCAN(nDeviceType, nDeviceInd, nCANInd, &vic);
    if (dwRel != 1)
    {
        std::cout << "init-fail:" << dwRel << std::endl;
        VCI_CloseDevice(nDeviceType, nDeviceInd);

        return FALSE;
    }
    else
    {
        std::cout << "initsuccess:" << dwRel << std::endl;
    }

    if (VCI_StartCAN(VCI_USBCAN2, 0, 0) != 1)
    {
        std::cout << "start-fail:" << dwRel << std::endl;
        VCI_CloseDevice(VCI_USBCAN2, 0);
    }
    else
    {
        std::cout << "startsuccess:1" << endl;
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "client_node");
    ros::NodeHandle nh;
    // ros::Rate loop_rate(MESSAGE_FREQ); // set the rate as defined in the macro MESSAGE_FREQ
    ros::Rate loop_rate(500);
    Listener listener;
    ros::Subscriber client_sub = nh.subscribe("/move_group/display_planned_path", 1000, &Listener::callback, &listener);

    init_can(); // 启动can分析仪open init start

    // 需要发送的帧，结构体设置
    /*
    VCI_CAN_OBJ send[1];
    send[0].ID = 1;
    send[0].SendType = 0;
    send[0].RemoteFlag = 0;
    send[0].ExternFlag = 0;
    send[0].DataLen = 5;

    int i = 0;

    send[0].Data[0]=28;
    for (i = 1; i < send[0].DataLen; i++)
    {
        send[0].Data[i] = 0;
    }


    int m_run0 = 1;
    pthread_t threadid;
    int ret;

    int times = 5;

    while (times--)
    {

        if (VCI_Transmit(VCI_USBCAN2, 0, 0, send, 1) == 1)
        {
            printf("Index:%04d  ", count);
            count++;
            printf("CAN1 TX ID:0x%08X", send[0].ID);
            if (send[0].ExternFlag == 0)
                printf(" Standard ");
            if (send[0].ExternFlag == 1)
                printf(" Extend   ");
            if (send[0].RemoteFlag == 0)
                printf(" Data   ");
            if (send[0].RemoteFlag == 1)
                printf(" Remote ");
            printf("DLC:0x%02X", send[0].DataLen);
            printf(" data:0x");

            for (i = 0; i < send[0].DataLen; i++)
            {
                printf(" %02X", send[0].Data[i]);
            }

            printf("\n");
            send[0].ID += 1;
        }
        else
        {
            break;
        }
    }
    */

    while (ros::ok())
    {
        // 获取缓冲区内的字节数
        // size_t n = sp.available();
        ros::spinOnce();
        loop_rate.sleep();
        uint8_t canidlist2[10] = {1, 2, 3, 4, 5, 6}, cmd2[10] = {30, 30, 30, 30, 30, 30};

        sendSimpleCanCommand(6, canidlist2, 8);
    }

    // 关闭串口

    return 0;
}