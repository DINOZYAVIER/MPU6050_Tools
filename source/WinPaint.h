#ifndef WIN_PAINT
#define WIN_PAINT

#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>   


class WinPaint

{
private:

	class Layout
	{
	public:
		static const COLORREF color = RGB(220, 220, 220);
		int height;
		int width;
		int vertical_x;
		int horizontal_y[3];

		//Layout(HDC hdc, const int& height, const int& width);
		//void paint();
		//void paintVerticalLine(const int& x);
		//void paintHoryzontalLine(const int& y);

		Layout(const int& height, const int& width)
		{
			this->height = height;
			this->width = width;

			vertical_x = width / 2;
			//int horizontal_y[] = { 120, height / 2 - 15, height - 150 };
			horizontal_y[0] = 120;
			horizontal_y[1] = height / 2 - 15;
			horizontal_y[2] = height - 150;
		}

		void paint(HDC hdc)
		{
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, color);

			paintVerticalLine(hdc, vertical_x);

			for (auto& y : horizontal_y)
			{
				paintHoryzontalLine(hdc, y);
			}

		}

		void paintVerticalLine(HDC hdc, const int& x)
		{
			MoveToEx(hdc, x, 0, NULL);
			LineTo(hdc, x, height);
		}

		void paintHoryzontalLine(HDC hdc, const int& y)
		{
			MoveToEx(hdc, 0, y, NULL);
			LineTo(hdc, width, y);
		}

	};

	class Text
	{
	public:
		const char* port = "Port";
		const char* bth = "Bluetooth";
		const char* tcpip = "TCP/IP";
		const char* acceleration = "Acceleration, m/s^2 (0 - 2g)";
		const char* gyro = "Gyro, deg/s (0 - 250)";
		const char* temperature = "Temperature, `C (0 - 100)";

		static const int port_y = 2;
		static const int gyro_y = 125;

		int left_x;
		 
		int accel_y;
		int temp_y;


		Text(const int& height, const int& left_x = 10)
		{
			this->left_x = left_x;
			accel_y = height / 2 - 10;
			temp_y = height - 145;
		}

		void paint(HDC hdc)
		{
			TextOut(hdc, left_x, port_y, port, strlen(port));
			TextOut(hdc, GetSystemMetrics(SM_CXSCREEN) / 6, port_y, tcpip, strlen(tcpip));
			TextOut(hdc, GetSystemMetrics(SM_CXSCREEN) / 1.5, port_y, bth, strlen(bth));
			TextOut(hdc, left_x, accel_y, acceleration, strlen(acceleration));
			TextOut(hdc, left_x, gyro_y, gyro, strlen(gyro));
			TextOut(hdc, left_x, temp_y, temperature, strlen(temperature));
		}
	};

	class DataGraphics
	{
	public:
		static const COLORREF graphColor = RGB(128, 128, 128);
		static const COLORREF clearColor = RGB(255, 255, 255);
		const COLORREF colorArray[3] = {
			RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255)
		};

		static const int dataLength = 7;
		static const int graphCount = 3;

		int current_fill_pos;
		int left_x;
		int right_x;
		int x_size;
		int y_size;
		int height;
		int width;
		int top_y[graphCount];
		int start_y[graphCount];
		double accelScale;
		double tempScale;
		RECT rects[graphCount];

		std::vector<int> dataArray;

		//DataGraphics(HDC hdc, const int& height, const int& width, const int& left_x = 10);
		/*void paint();
		void paintGraphic(const int& left_x, const int& top_y, const int& right_x, const int& bottom_y);
		void clean();
		void cleanGraphic(const RECT& rt);
		void update(const std::string& data);
		void updateDataArray(const std::string& data);
		void fill();
		void setPoint(const int& index, const int& graph_number, const int& color_number);*/

		DataGraphics(const int& height, const int& width, const int& left_x = 10)
		{
			this->height = height;
			this->width = width;
			this->left_x = left_x;

			current_fill_pos = 0;
			x_size = width / 2 - 20;
			y_size = height / 2 - 170;
			right_x = left_x + x_size;

			//top_y = { 145, height / 2 + 10, height - 125 };
			top_y[0] = 145;
			top_y[1] = height / 2 + 10;
			top_y[2] = height - 125;

			//start_y[] = { height / 2 - 25, height - 160, height - 10 };
			start_y[0] = height / 2 - 25;
			start_y[1] = height - 160;
			start_y[2] = height - 10;

			accelScale = static_cast<double>(start_y[0] - top_y[0]) / static_cast<double>(1 << 15);
			tempScale = 1; 

			int temp = width / 2;
			for (int i = 0; i < graphCount; ++i)
			{
				rects[i] = { left_x, top_y[i], right_x + 1, start_y[i] + 1 };
			}
		}

		void paint(HDC hdc)
		{
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, graphColor);

			for (int i = 0; i < graphCount; ++i)
			{
				paintGraphic(hdc, left_x - 1, top_y[i], right_x, start_y[i] + 1);
			}
		}

		void paintGraphic(HDC hdc, const int& left_x, const int& top_y, const int& right_x, const int& bottom_y)
		{
			MoveToEx(hdc, left_x, top_y, NULL);
			LineTo(hdc, left_x, bottom_y);
			LineTo(hdc, right_x, bottom_y);
		}

		void clean(HDC hdc)
		{
			for (auto& rt : rects)
			{
				cleanGraphic(hdc, rt);
			}
		}

		void cleanGraphic(HDC hdc, const RECT& rt)
		{
			FillRect(hdc, &rt, (HBRUSH)clearColor);
		}

		void update(HDC hdc, const std::string& data)
		{
			updateDataArray(data);
			fill(hdc);
		}

		void updateDataArray(const std::string& data)
		{
			std::stringstream ss(data);
			int value;
			int count = 0;

			for (int count = 0; count < dataLength && ss >> value; ++count)
			{
				value = scaleValue(std::abs(value), count);
				if (dataArray.size() == count)
					dataArray.push_back(value);
				else
					dataArray[count] = value;
			}
		}

		void resetPosition()
		{
			current_fill_pos = x_size;
		}

		void fill(HDC hdc)
		{
			if (current_fill_pos++ == x_size)
			{
				current_fill_pos = 0;
				clean(hdc);
			}

			for (int i = 0; i < dataArray.size(); ++i)
			{
				setPoint(hdc, i, i / 3, i % 3);
			}
		}

		void setPoint(HDC hdc, const int& index, const int& graph_number, const int& color_number)
		{
			SetPixel(hdc, left_x + current_fill_pos, start_y[graph_number] - dataArray[index],
				colorArray[color_number]);
		}

		int scaleValue(const int &value, int count)
		{
			double scale = accelScale;
			if (6 == count)
				scale = tempScale;

			return static_cast<double>(value * scale);
		}
	};

public:
	enum { INIT = 1, SOCKET = 2, BLUETOOTH = 4 };

	HWND hWnd;
	HDC hdc;

	Layout* layout;
	Text* text;
	DataGraphics* socketGraph;
	DataGraphics* bluetoothGraph;

	int height;
	int width;

	WinPaint(HWND hWnd)
	{
		this->hWnd = hWnd;
	}

	~WinPaint()
	{
		delete layout;
		delete text;
		delete socketGraph;
		delete bluetoothGraph;
	}

	void initialPaint()
	{
		RECT rt;
		GetClientRect(hWnd, &rt);
		height = rt.bottom;
		width = rt.right;

		if (!layout)
			layout = new Layout(height, width);

		if (!text)
			text = new Text(height);

		if (!socketGraph)
			socketGraph = new DataGraphics(height, width);

		if (!bluetoothGraph)
			bluetoothGraph = new DataGraphics(height, width, width / 2 + 10);

		layout->paint(hdc);
		text->paint(hdc);
		socketGraph->paint(hdc);
		bluetoothGraph->paint(hdc);
	}

	void onPaint(const int& state, const std::string& dataSocket, const std::string& dataBluetooth)
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);

		if (state & INIT)
		{
			initialPaint();
			// TODO - Find the better way to paint graphics after redrawing.
			socketGraph->resetPosition();
			bluetoothGraph->resetPosition();
		}

		if (state & SOCKET && !dataSocket.empty())
		{
			socketGraph->update(hdc, dataSocket);
		}

		if (state & BLUETOOTH && !dataBluetooth.empty())
		{
			bluetoothGraph->update(hdc, dataBluetooth);
		}

		EndPaint(hWnd, &ps);
	}

};

#endif
