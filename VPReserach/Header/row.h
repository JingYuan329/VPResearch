#pragma once
#include<map>
#include"cell.h"
using namespace std;
class row
{
public:
	row(string NAME, string _X, string _Y, string _ORI, string DO, string BY, string _step_one, string _step_two) :
		_row_name(NAME), _row_site_x(stod(_X)), _row_site_y(stod(_Y)), _ORIENT(_ORI), _DO(stod(DO)), _BY(stod(BY)), _STEP { stod(_step_one), stod(_step_two)
}  {}

	string get_row_name() { return _row_name; };
	double get_row_site_x() { return _row_site_x; };
	double get_row_site_y() { return _row_site_y; };
	double get_row_site_x_end() { return _row_site_x+ _DO* _STEP.first; };					//不確定是不是這樣
	string get_ROW_orient() { return _ORIENT; };

	map<string,cell*>_this_row_cells;
	string _begin_cell;					//此row第一個cell 的名字
	string _end_cell;						//此row最後一個cell 的名字	


private:
	string _row_name;
	double _row_site_x = 0;
	double _row_site_y = 0;
	double _DO;
	double _BY;
	string _ORIENT;
	pair<double, double>_STEP;
};

