#pragma once
#include<iostream>
#include<vector>
#include<string>
using namespace std;

class RECT {

public:
	double & Point(int _p, char _X_or_Y) {
		if (_p == 1) {
			if (_X_or_Y == 'x') return _RECT_.first.first;
			else if (_X_or_Y == 'y') return _RECT_.first.second;
		}
		else if (_p == 2) {
			if (_X_or_Y == 'x') return _RECT_.second.first;
			else if (_X_or_Y == 'y') return _RECT_.second.second;
		}
	};
	bool operator==(const RECT& other) {
		if ((other._RECT_.first.first == _RECT_.first.first) &&
			(other._RECT_.first.second == _RECT_.first.second) &&
			(other._RECT_.second.first == _RECT_.second.first) &&
			(other._RECT_.second.second == _RECT_.second.second)) {
			return true;
		}
		else return false;
	};
	int horizontal_or_straight() {
		if ((_RECT_.second.first - _RECT_.first.first) > (_RECT_.second.second - _RECT_.first.second))   return 0;				// horizontal
		else return 1;					//vertical
	};

	int _access_flag = 0;
	//get_point(2, x) = 200.200;
private:
	pair<pair<double, double>, pair<double, double>>_RECT_;

};



class pin_shape 
{

public:
	// like  0.000 -0.040 1.440 0.040 ;
	double get_rect_point(string _Min_or_Max, char _X_or_Y) {
		double _min_x = 10000000, _min_y = 10000000, _max_x = 0,  _max_y = 0;

		for (int i = 0; i < _rect.size(); ++i) {
			if (_rect.at(i).Point(1, 'x') < _min_x)   _min_x = _rect.at(i).Point(1, 'x');
			if (_rect.at(i).Point(1, 'y') < _min_y)    _min_y = _rect.at(i).Point(1, 'y');
			if (_rect.at(i).Point(2, 'x') > _max_x)     _max_x = _rect.at(i).Point(2, 'x');
			if (_rect.at(i).Point(2, 'y') > _max_y)    _max_y = _rect.at(i).Point(2, 'y');
		}

		if (_Min_or_Max == "min") {
			if (_X_or_Y == 'x')   return _min_x;
			else if (_X_or_Y == 'y')   return _min_y;
		}
		else if (_Min_or_Max == "max") {
			if (_X_or_Y == 'x')   return _max_x;
			else if (_X_or_Y == 'y')  return _max_y;
		}
	};

	void set_one_rect(string _rect1, string _rect2, string _rect3, string _rect4) {
		RECT A;
		A.Point(1, 'x') = stod(_rect1);
		A.Point(1, 'y') = stod(_rect2);
		A.Point(2, 'x') = stod(_rect3);
		A.Point(2, 'y') = stod(_rect4);
		_rect.push_back(A);
	};
	void set_rect(RECT _a_RECT) { _rect.push_back(_a_RECT); };
	void clear_rect() { _rect.clear(); };
	vector<RECT>get_rect() { return _rect; };
	int get_rect_size() { return _rect.size(); };
	
private:
	vector<RECT>_rect;
};



class pin
{

public:
	pin(string NAME,string DIR,string USE):
		_pin_name(NAME), _dir(DIR), _use(USE){}
	pin() {}
	string getname() { return _pin_name; };
	string getdir() { return _dir; };
	string getuse() { return _use; }
	void setlayer(string LAYER) { _layer.push_back(LAYER); };
	vector<string> getlayer() { return _layer; };
	void _clear_layer() { _layer.clear(); };

	void Set_One_rect(string _rect1, string _rect2, string _rect3, string _rect4) {
		_pin_shape.set_one_rect(_rect1, _rect2, _rect3, _rect4);
	};
	void Set_rect(RECT _a_RECT) { //_rect.push_back(_a_RECT); 
		_pin_shape.set_rect(_a_RECT);
	};
	void Clear_rect() { _pin_shape.clear_rect(); };
	vector<RECT>Get_rect() { return _pin_shape.get_rect(); };
	int Get_rect_size() { return _pin_shape.get_rect_size(); };
	RECT find_rect(int _M2_dir) {
			for (int i = 0; i < Get_rect_size(); ++i) {
				if ((Get_rect().at(i).horizontal_or_straight()== _M2_dir)&& (Get_rect().at(i)._access_flag==0)){
					Get_rect().at(i)._access_flag = 1;
					return Get_rect().at(i);
				}
			}

		/*	for (int i = 0; i < Get_rect_size(); ++i) {
				if (Get_rect().at(i)._access_flag == 0) {
					Get_rect().at(i)._access_flag = 1;
					return Get_rect().at(i);
				}
			}*/
				RECT trect;
				trect._access_flag = -1;
			return trect;

	};

	void _reset_all_RECT_access() {
		for (int i = 0; i < Get_rect_size(); ++i) {
			Get_rect().at(i)._access_flag = 0;
		}
	};


	////obs
	vector<RECT>_obs;                 //以前的VP資訊裝在這裏面。現在VP資訊裝在每個cell內
	vector<string>_obs_layer;

private:
	string _pin_name;
	string _dir;				  //INPUT or OUTPUT     // INOUT
	string _use;				//SIGNAL  or POWER or GROUND
	//string _layer;				// ex: Metal1     //if  PIN o:  做落成2個RECT, 有可能兩個RECT在不同層----> vector<string> _layer
	vector<string> _layer;
	pin_shape _pin_shape;       //vector<RECT>_rect;
	////obs

	//////////////pin 需不需要記得自己的cell type名字

};