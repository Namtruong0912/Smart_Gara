// Dự án nhỏ này giới hạn chỗ để xe là 5 và biển số là loại 4 số (9 kí tự)

//----------------------------------------------Thư viện---------------------------------------------------
#include"Park.h"
#include "main.h"
#include "stm32f1xx.h"

//----------------------------------------------Cấu hình ngoại vi------------------------------------------
// Dùng pwm của timer2 để điều khiển servo đóng mở barie
extern TIM_HandleTypeDef htim2;

// Dùng uart2 để giao tiếp với máy tính, nhận dữ liệu biển số
extern UART_HandleTypeDef huart2;

// DÙng uart3 để giao tiếp với module đọc thẻ RFID, nhận dữ liệu thẻ RFID
extern UART_HandleTypeDef huart3;

//----------------------------------------------Biến toàn cục----------------------------------------------
// Tín hiệu yêu cầu xử lý ảnh
char* senddata = "1";

// Lưu mã thẻ tạm thời
int RFID_COUNT;
int RFID_DATA;
int RFID_BUFF[6];

// Lưu biển số tạm thời
int COM_COUNT;
int COM_DATA;
int COM_BUFF[9];

// Biến trong vòng lặp for
int i;
int j;
int u;

// Lưu mã thẻ và biển số lâu dài
long RFID_SAVE[30];
long COM_SAVE[45];

// Mảng lưu vị trí trống trong bãi đỗ
int empty_array[5];

// Biến trạng thái kiểm tra vị trí trống
int empty;

// Biến trạng thái điều khiển barie
int barie;

// Biến trạng thái kiểm tra biển số đã được lưu chưa, nếu biển số này được lưu nghĩa là xe đã ở trong nhà xe trước đó và chuẩn bị đi ra
int save_status;

// Biến trạng thái kiểm tra mã thẻ có trùng khớp với với mã thẻ đã được lưu trước đó hay không
int overlap_status;

// Biến thể hiện vị trí trùng lặp - vị trí mà mã thẻ và biển số đã được lưu trong mảng
int location_overlap;



//--------------------------------------------Khai báo nguyên mẫu hàm--------------------------------------
/*
 * Tên hàm: buzzer(int n)
 * Chức năng: Điều khiển còi báo
 * Tham số vào: số lần còi kêu
 * Tham số trả về: Không có
 */
void buzzer(int n);


/*
 * Tên hàm: save_status_check()
 * Chức năng: Kiểm tra xem biển số này đã được lưu trước đó chưa
 * Tham số vào: Không có
 * Tham số trả về: trạng thái lưu (save_status) và địa chỉ trùng lặp nếu đã được lưu (location_overlap)
 */
void save_status_check()


/*
 * Tên hàm: car_in()
 * Chức năng: Lưu lại biển số và mã thẻ
 * Tham số vào: Không có
 * Tham số trả về: Biến trạng thái đóng mở barie (barie)
 */
void car_in();


/*
 * Tên hàm: car_out()
 * Chức năng: Kiểm tra sự trùng khớp của mã thẻ và biển số hiện tại với dữ liệu đã được lưu trước đó
 * Tham số vào: Không có
 * Tham số trả về: Biến trạng thái đóng mở barie (barie)
 */
void car_out();


//-------------------------------------------Định nghĩa các hàm--------------------------------------------
void buzzer(int n)
{
	for(i=0; i<n; i++)
	{
		HAL_GPIO_WritePin(Buzzer_pin_GPIO_Port, Buzzer_pin_Pin, 1);
		HAL_Delay(200);
		HAL_GPIO_WritePin(Buzzer_pin_GPIO_Port, Buzzer_pin_Pin, 0);
		HAL_Delay(200);
	}
}


void save_status_check()
{
	save_status = 0;
	// So sáng biển số với cả 5 vị trí xem biển số này đã được lưu trước đó chưa
	for(i=0; i<5; i++)
	{
		for(j=0; j<9; j++)
		{
			if(COM_BUFF[j] != COM_SAVE[i*9 + j]) break;
			else
			{
				if(j == 8)
				{
					save_status = 1;
					location_overlap = i;
				}
			}
		}
	}
}


void car_in()
{
	// Kiểm tra và đưa ra vị trí trống trong bãi đỗ
	for(i=4; i>=0;i--)
	{
		if(empty_array[i] == 0)
		{
			empty = i;
		}
	}
	empty_array[empty] = 1;

	// Lưu lại mã thẻ và biển số, đồng thời đặt biến trạng thái mở barie cho xe vào
	for(i=0; i<=5; i++)
	{
		RFID_SAVE[empty*6 + i] = RFID_BUFF[i];
	}
	for(i=0; i<=8; i++)
	{
		COM_SAVE[empty*9 + i] = COM_BUFF[i];
	}
	barie = 1;
}


void car_out()
{
	overlap_status = 0;
	for(i=0; i<6; i++)
	{
		if(RFID_SAVE[location_overlap*6 + i] == RFID_BUFF[i])
		{
			if(i == 5)
			{
				// Nếu thẻ RFID trùng khớp thì đặt biến trạng thái mở barie cho xe ra đồng thời xóa dữ liệu thẻ RIFD và biển số
				overlap_status = 1;
				barie = 1;
				for(i=0; i<6; i++)
				{
					RFID_SAVE[location_overlap*6 + i] = 0;
				}
				for(i=0; i<9; i++)
				{
					COM_SAVE[location_overlap*9 + i] = 0;
				}
				empty_array[location_overlap] = 0;
			}
		}
		else break;
	}
}


// Hàm thực thi của ngắt UART, được gọi ra khi có sự kiện ở ngoại vi UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	  {
		  if(huart -> Instance == USART3)
		  {
			  if(RFID_COUNT<6)
			  {
				  RFID_BUFF[RFID_COUNT] = RFID_DATA;
				  RFID_COUNT++;
			  }
			  if(RFID_COUNT==6)
			  {
				  RFID_COUNT=0;
				  HAL_UART_Transmit(&huart2, senddata, 1, 100);
			  }
			  HAL_UART_Receive_IT(&huart3, &RFID_DATA, 1);
		  }

		  if(huart -> Instance == USART2)
		  {
			  if(COM_COUNT<9)
			  {
				  COM_BUFF[COM_COUNT] = COM_DATA;
				  COM_COUNT++;
			  }
			  if(COM_COUNT==9)
			  {
				  COM_COUNT=0;
				  save_status_check();
				  if(save_status == 0)
				  {
					  // Nếu biển số chưa được lưu
					  car_in();
				  }
				  else
				  {
					  // Nếu biển số đã được lưu
					  car_out();
				  }
			  }
			  HAL_UART_Receive_IT(&huart2, &COM_DATA, 1);
		  }
	  }

