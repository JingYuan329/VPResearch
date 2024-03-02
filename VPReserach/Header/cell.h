#pragma once
#include"cell_type.h"
#include<iostream>
#include<string>

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

using namespace std;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<box, unsigned> value;
class cell_type;

class cell
{
public:
	cell(string NAME1, string NAME2, string _X,string _Y,string _ORIENT,string _PLACED_OR_FIXED) :
		_cell_type_name(NAME1), _cell_name(NAME2), _x(stod(_X)), _y(stod(_Y)), _orient(_ORIENT), _placed_or_fixed(_PLACED_OR_FIXED){}

	friend class design;

	string get_cell_type_name() { return _cell_type_name; };
	string get_cell_name() { return _cell_name; };
	double get_x() { return _x; };
	double get_y() { return _y; };
	string get_orient() { return _orient; };
	string get_placed_or_fixed() { return _placed_or_fixed; };

	void set_parent_cell_type(string _PARENT) { _parent =_PARENT; };       
	string get_parent_cell_type() { return _parent; };
	void set_which_row_name(string _ROW_NAME) { _which_row = _ROW_NAME; };       
	string get_which_row_name() { return _which_row; };
	cell_type* get_cell_type() { return ct; };                         
	void set_cell_type(cell_type* CT) { ct = CT; };

	void reset_this_pin() {
		if (get_cell_type()->find_outpin() != NULL) {
			get_cell_type()->find_outpin()->_reset_all_RECT_access();
		}
	};


	////////////////there's no need any more  (using rtree will build all pins' position at first)
	cell* _up_cell = NULL;					//有可能是兩個!!!!!!!!!!!!!!!!!!
	cell* _down_cell = NULL;				//有可能是兩個!!!!!!!!!!!!!!!!!!
	pair<cell*, double> _left_cell;          //double: 距離多遠
	pair<cell*, double> _right_cell;
	////////////////////////////////////////////

	//vector<pin*> get_pins() {
	//	
	//};

	
	vector<RECT>_VP;
	vector<string>_VP_layer;
	string _new_cell_type_name="";
	string _VP_pin_name = "";

	vector<value> _buffer_overlap_m2VP;
	vector<value> _buffer_overlap_m3PG;
	vector<value> _buffer_overlap_m4PG;

private:
	string _cell_type_name;
	string _cell_name;
	double _x = 0;
	double _y = 0;
	string _orient;
	string _placed_or_fixed;
	string _parent;
	string _which_row;

	cell_type* ct;
};
