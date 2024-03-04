#pragma once
#include"cell_type.h"
#include "layer.h"
#include "row.h"

#include<algorithm>
#include<utility>
#include <iomanip>
#include<fstream>
#include <deque>

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/algorithms/overlaps.hpp>
#include <boost/geometry/geometries/polygon.hpp>


using namespace std;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<box, unsigned> value;
typedef bgi::rtree< value, bgi::quadratic<16>> rTree;
typedef bg::model::polygon<point> polygon;
//typedef std::pair<box, unsigned> value;

class design {
public:
	void parser_tech_lef();
	void parser_cell_lef(string filename);       //ispd18 lef :  the first : spacing pitch  havn't parser 
	void parser_def(string filename);

	void insert_VP_straight_initial(string which_cell_type);      //straight , up
	vector<RECT>BuildVP(int dir,  vector<RECT>& pre_vp, double _wire_width, int _slice);
	///////////////////////////////////////////////////////////////////
	//void insert_VP_skew_initial(string which_cell_type);      //skew , up
	void insert_all_VP_initial();
	void insert_some_VP_initial(double percent);
	//pair<pair<pair<double,double>,pair<double,double>>>
	void dodge_pin_VP( cell* which_cell);
	void insert_dodge_VP(double percent);
	RECT choose_base_pin_rect(cell_type* which_cell_type,int _M2_dir);
	RECT choose_specify_dir_pin_rect(cell_type* which_cell_type, int _dir);
	bool check_overlap(box range, vector<value>check_array, vector<value>&_result);

	
	void offset(vector<pair<pair<double, double>, pair<double, double>>>& _now_vp, double _bias, double _low_bound, double _up_bound);

	void sort_cell_on_row();
	void output_lef_file(string outfilename);
	void output_dodge_lef_file(string outfilename);
	void output_dodge_def_file(string outfilename);
	void cout_cell_lef();
	void cout_def();




	//R_tree
	map<string,rTree*> forest;        //M1-M8's tree's name;    M1-M8's tree
	vector<pair<string,string>> rTree_value;
	// insert new value : a M1 cell's one pin in RTREE 
	//RTREE->insert(make_pair(b, make_pair(_cellName, _pin_name)));     --->  RTREE->insert(make_pair(b, vector index ))
	void make_rtree_M1();
	vector<value> oneMetal_VP_overlap(string _MET_LAYER, string _CELL_NAME, string _PIN_NAME, box  _new_VP);		// (VP_overlap : m1-m8 will be right)
	void make_PG_rtree_M3M4();
	void add_rtree(string _Metal, string _CELL_NAME, string _PIN_NAME,box _new_rect);

	box FineTuine(const box &_nowVP, const vector<value> & overlap_box);

	int caseC_count = 0;

private:
	double _units=0;

	////////////////////no need but in parser_tech_lef  ( ispd15_pic_bridge)  /////////////////////////
	//map<string, int>_all_layer; //Metal1->0 (Horizontal)
	vector<pair<int, double>>_layer_metal;   //Horizontal=0; Vertical=1;       // LAYER metal1 : 0 .... LAYER metal5 : 4
	vector<double>_via_layer_spacing;      //LAYER via1 : 0 ....LAYER via4 : 3
	vector<double>_metal_layer_pitch;     // LAYER 1 : 0 ....LAYER 5 : 4
	vector<double>_metal_layer_width;     //LAYER 1 : 0 ....LAYER 5 : 4
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	map<string, layer*>_all_layer;
	map<string, cell_type*>_all_cell_type_new;
	//_all_cell_type A;
	map<string, cell*>_all_cell;      //不知道要不要建這個 目前還用不到
	map<double, row*>_all_row;    //each row's  y(height)
	//map<string, pin*>_pins_of_obs;

	pair<pair<double, double>, pair<double, double>> _diearea;
	int _components_num = 0;

	vector<vector<string>>_VDD_VSS;			//parser input  :  VDD(VSS),  Metal3,  400,  20000,  20200,  1800000,  20200


};

