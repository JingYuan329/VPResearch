#pragma once
#include"pin.h"
#include"cell.h"
using namespace std;
class cell_type
{
public:
	cell_type(string NAME,string SIZE,string BY) :
		_cell_type_name(NAME), _size(stod(SIZE)),_by(stod(BY)){}

	string get_cell_type_name() { return _cell_type_name; };
	double get_size() { return _size; };
	double get_by() { return _by; };

	vector<pin*>_all_pin;
	vector<cell*>_this_type_all_cell;         //目前不需要使用map，但不知道之後會不會需要
	vector<pin*>_all_OBS;               // 以前裝真的obs，現在用不到了
	bool _has_insert_VP=0;              //default 都是沒有insert VP的cell type

private:
	string _cell_type_name;
	double _size = 0;                    //_length
	double _by = 0;				// _width
	//ORIGIN
	//SYMMETRY

	//obs
};
