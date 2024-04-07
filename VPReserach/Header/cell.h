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
		//ct->find_outpin();
		//get_cell_type();
		//get_cell_type()->find_outpin();
		//if (get_cell_type()->find_outpin() != NULL) {
		//	get_cell_type()->find_outpin()->_reset_all_RECT_access();
		//}
	};


	vector<value>& get_buffer_overlap_m2VP() { return _buffer_overlap_m2VP; };
	vector<value>& get_buffer_overlap_m3PG() { return _buffer_overlap_m3PG; };
	vector<value>& get_buffer_overlap_m4PG() { return _buffer_overlap_m4PG; };
	void set_buffer_overlap_m2VP(vector<value> _m2VP) { _buffer_overlap_m2VP =_m2VP; };
	void set_buffer_overlap_m3PG(vector<value> _m3PG) { _buffer_overlap_m3PG= _m3PG; };
	void set_buffer_overlap_m4PG(vector<value> _m4PG) { _buffer_overlap_m4PG = _m4PG; };
	void set_buffer_overlap_m3PG(vector<value> _m3PG, double _buffer) {
		for (int i = 0; i < _m3PG.size(); ++i) {
			_m3PG.at(i).first.min_corner().set < 0 >(_m3PG.at(i).first.min_corner().get < 0 >() - _buffer);
			_m3PG.at(i).first.min_corner().set < 1 >(_m3PG.at(i).first.min_corner().get < 1 >() - _buffer);
			_m3PG.at(i).first.max_corner().set < 0 >(_m3PG.at(i).first.max_corner().get < 0 >()+ _buffer);
			_m3PG.at(i).first.max_corner().set < 1 >(_m3PG.at(i).first.max_corner().get < 1 >() + _buffer);
		}
		_buffer_overlap_m3PG =_m3PG;
	};
	void set_buffer_overlap_m4PG(vector<value> _m4PG, double _buffer) {
		for (int i = 0; i < _m4PG.size(); ++i) {
			_m4PG.at(i).first.min_corner().set < 0 >(_m4PG.at(i).first.min_corner().get < 0 >() - _buffer);
			_m4PG.at(i).first.min_corner().set < 1 >(_m4PG.at(i).first.min_corner().get < 1 >() - _buffer);
			_m4PG.at(i).first.max_corner().set < 0 >(_m4PG.at(i).first.max_corner().get < 0 >() + _buffer);
			_m4PG.at(i).first.max_corner().set < 1 >(_m4PG.at(i).first.max_corner().get < 1 >() + _buffer);
		}
		_buffer_overlap_m4PG = _m4PG;
	};


	////////////////there's no need any more  (using rtree will build all pins' position at first)
	cell* _up_cell = NULL;					//有可能是兩個!!!!!!!!!!!!!!!!!!
	cell* _down_cell = NULL;				//有可能是兩個!!!!!!!!!!!!!!!!!!
	pair<cell*, double> _left_cell;          //double: 距離多遠
	pair<cell*, double> _right_cell;
	////////////////////////////////////////////
	
	vector<RECT>_VP;
	vector<string>_VP_layer;
	string _new_cell_type_name="";
	string _VP_pin_name = "";

private:
	string _cell_type_name;
	string _cell_name;
	double _x = 0;
	double _y = 0;
	string _orient;
	string _placed_or_fixed;
	string _parent;
	string _which_row;
	vector<value> _buffer_overlap_m2VP;
	vector<value> _buffer_overlap_m3PG;
	vector<value> _buffer_overlap_m4PG;

	cell_type* ct;
};
