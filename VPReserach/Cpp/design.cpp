#include "design.h"
#include<iostream>
using namespace std;
using namespace std;

void design::parser_tech_lef()
{
	pair<int, double>temp_metal;
	_units = 1000;
	for (int i = 0; i < 5; ++i){                          //手動寫入layer metal的方向和PITCH    //寫死
		temp_metal.first = i % 2;
		temp_metal.second = 0.200;
		_layer_metal.push_back(temp_metal);
	}

	for (int i = 0; i < 4; ++i) {                          //手動寫入 via layer的SPACING
		_via_layer_spacing.push_back(0.10);
	}

	for (int i = 0; i < 5; ++i) {                          //手動寫入 layer的pitch
		_metal_layer_pitch.push_back(0.20);
	}

	for (int i = 0; i < 5; ++i) {                          //手動寫入layer的width
		_metal_layer_width.push_back(0.10);
	}

}
void design::parser_cell_lef(string filename)
{
	ifstream fin;
	string temp,temp_name,temp2,LA_NAME, temp3,rect1,rect2,rect3,rect4;
	cout << "start parser cell lef " << endl;
	fin.open(filename, ios::in);
	if (!fin.is_open())  cout << "cell.lef is not open ! " << endl;
	else {
		while (fin >> temp) {
			if (temp == "MACRO") {
				fin >> temp_name;
				fin >> temp;				//CLASS
				fin >> temp;				//BLOCK; or CORE;
				if (temp == "BLOCK") {
					;
				}
				else if (temp == "CORE") {
					fin >> temp;				//;
					while (temp != "SIZE") {
						fin >> temp;				//FOREIGN  ms00f80  ;    ORIGIN   0  0  ;  SIZE
					}
					fin >> temp;     //SIZE num
					fin >> temp2;     //"BY"
					fin >> temp2;     //BY num

					cell_type * _tempcell_type = new cell_type(temp_name,temp,temp2);
					while (temp != "PIN"&&temp!="OBS") {
						fin >> temp;    //;  SYMMETRY  X  Y  R90  ;  SITE   core    ; PIN
					}

					while ((temp == "PIN") || (temp == "OBS")) {
						if (temp == "PIN") {
							fin >> temp;   //PIN's name : o
							fin >> temp2;    //"DIRECTION"
							while (temp2 != "DIRECTION") {
								fin >> temp2;
							}
							fin >> temp2;    //"OUTPUT"
							fin >> temp3;    //";"
							while (temp3 != "USE") {
								fin >> temp3;
							}
							fin >> temp3;       //"SIGNAL  or  POWER"
							pin* _temp_pin = new pin(temp, temp2, temp3);
							while (temp2 != "LAYER") {
								fin >> temp2;    //;   //"PORT"   //"LAYER"
										  //  #  USE SIGNAL ;
							}
							while (temp2 == "LAYER") {
								fin >> LA_NAME;     //metal2  or metal1
								fin >> temp;     //;

									   //if  PIN o:  做落成2個RECT, 有可能兩個RECT在不同層  , 這裡會有錯
								fin >> temp2;    //RECT
								while (temp2 == "RECT") {
									fin >> rect1;  fin >> rect2;  fin >> rect3;  fin >> rect4;
									_temp_pin->Set_One_rect(rect1, rect2, rect3, rect4);
									_temp_pin->setlayer(LA_NAME);
									fin >> temp2;    //;
									fin >> temp2;    //"RECT"  or  "END"  or  "LAYER"
									if (temp2 == "#") {
										cout << "temp2==# !!!!!!" << endl;
										getline(fin, temp2);
										fin.ignore();
										fin >> temp2;      //END
										cout << "First END " << endl;
									}
								}
							}
							fin >> temp2;  //second "END"
							fin >> temp2;   //"o" (PIN name)

							_tempcell_type->_all_pin.push_back(_temp_pin);

							fin >> temp;   // PIN  or   //END  or  //OBS
						}     //if (temp == "PIN") 


						else if (temp == "OBS") {
							fin >> temp2;
							pin* _temp_obs_pin = new pin;
							while (temp2 != "LAYER") {
								fin >> temp2;
							}
							while (temp2 == "LAYER") {
								fin >> LA_NAME;     //metal2  or metal1
								fin >> temp;     //;

								fin >> temp2;    //RECT
								while (temp2 == "RECT") {
									fin >> rect1;  fin >> rect2;  fin >> rect3;  fin >> rect4;
									_temp_obs_pin->Set_One_rect(rect1, rect2, rect3, rect4);
									_temp_obs_pin->setlayer(LA_NAME);
									fin >> temp2;    //;
									fin >> temp2;    //"RECT"  or  "END"  or  "LAYER"
									if (temp2 == "#") {
										cout << "temp2==# !!!!!!" << endl;
										getline(fin, temp2);
										fin.ignore();
										fin >> temp2;      //END
										cout << "First END " << endl;
									}
								}
							}
							_tempcell_type->_all_OBS.push_back(_temp_obs_pin);

							fin >> temp;   // PIN  or   //END  or  //OBS
						}			//	else if (temp == "OBS")


					}        //while ((temp == "PIN") || (temp == "OBS"))
					fin >> temp2; //MACRO's name

					_all_cell_type_new.insert({ temp_name, _tempcell_type });
					//fin >> temp2;     //"MACRO"
				}
			//	cout << "temp_name: " << temp_name << endl;
			}
		
			else if (temp =="LAYER" ) {
				fin >> temp;                              //LAYER name: Metal1
				if (temp.at(0) == 'M') {     //temp="METAL1"
					fin >> temp2;    //TYPE   //;
					if (temp2 == "TYPE") {
						while (temp2 != "DIRECTION") {
							fin >> temp2;
						}
						fin >> temp2;     //HORIZONTAL    //VERTICAL
						fin >> rect1;
						while (rect1 != "WIDTH") {
							fin >> rect1;
						}
						fin >> rect1;               //WIDTH : "0.070000"
						fin >> rect2;
						while (rect2 != "SPACING") {
							fin >> rect2;
							//cout << "rect2: " << rect2 << endl;
							//cout << __LINE__ << endl;
						}
						fin >> rect2;               //SPACING : "0.070000"
						layer* _temp_layer = new  layer(temp, temp2, rect1, rect2);
						_all_layer.insert({ temp,_temp_layer });
					}
				}     //end if (temp.at(0) == 'M') { 
			}
		
		}

		
	}
	
}
void design::parser_def(string filename)
{
	ifstream fin;
	string temp, temp_name, temp_type,temp2, rect1, rect2, rect3, rect4,r5,r6;
	string _which_pg, _p_or_f;
	cout << "start parser  def " << endl;
	fin.open(filename, ios::in);
	if (!fin.is_open())  cout << "cell.lef is not open ! " << endl;
	else {

		while (fin >> temp) {
			if (temp == "DIEAREA") {
				fin >> temp;     //"("
				fin >> rect1;    //0
				fin >> rect2;     //0 
				fin >> temp;     //")"
				fin >> temp;     //"("
				fin >> rect3;      //800000
				fin >> rect4;      //800000
				fin >> temp;     //")"
				fin >> temp;     //";"
				_diearea.first.first = stod(rect1);
				_diearea.first.second = stod(rect2);
				_diearea.second.first = stod(rect3);
				_diearea.second.second = stod(rect4);
			}
			else if (temp == "UNITS") {
				fin >> temp;				//DISTANCE
				fin >> temp;				//MICRONS
				fin >> temp;				//2000
				_units = stod(temp);
			}
			else if (temp == "ROW") {
				fin >> temp_name;			 //"CORE_ROW_0"
				fin >> temp;    //CoreSite
				fin >> rect1;    //83600
				fin >> rect2;    //71820
				fin >> temp2;   //N
				fin >> temp;   //DO
				fin >> rect3;     //52
				fin >> temp;   //BY
				fin >> rect4;      //1
				fin >> temp;   //STEP
				fin >> r5; fin >> r6;    //400 0
				row* _temp_row = new row(temp_name, rect1, rect2, temp2, rect3, rect4, r5, r6);
				_all_row.insert({ stod(rect2),_temp_row });
			}
			else if (temp == "COMPONENTS") {
				fin >> temp;
				_components_num = stoi(temp);
				fin >> temp;   //";"
				fin >> temp;     //"-"
				while (temp != "END") {
					
					fin >> temp_name;    //h6/g60632_u1      //
					fin >> temp_type;     //na02f01                    //
					fin >> temp;     //"+"
					fin >> _p_or_f;     //"PLACED"   or   "SOURCE"
					if (_p_or_f == "SOURCE") {
						fin >> temp;      //"TIMING"
						fin >> temp;    //"+"
						fin >> _p_or_f;    //"PLACED"
					}
					fin >> temp;     //"("
					fin >> rect1;     //283000                              //
					fin >> rect2;      //196000       //row _y      //
					fin >> temp;     //")"
					fin >> temp2;     //"N"                                    //

					fin >> temp;    //";"
					fin >> temp;     //"-"    or "END"

				//  if 可以在_all_cell_type_new 內找到此cell_type
				//new 一新cell , 加入  temp_type
					//cell.lef重接 成map:  _all_cell_type_new

					auto iter = _all_cell_type_new.find(temp_type);
					if (iter != _all_cell_type_new.end()) {
						cell* _tempcell = new cell(temp_type, temp_name, rect1, rect2, temp2, _p_or_f);
						_tempcell->set_parent_cell_type(temp_type);                            //set this cell's cell_type (string)
						_tempcell->set_cell_type(iter->second);						    //set this cell's cell_type (pointer)

						_all_cell_type_new[temp_type]->_this_type_all_cell.push_back(_tempcell);       //set this cell's cell_type
						_tempcell->set_which_row_name(_all_row[stod(rect2)]->get_row_name());       //set this cell is on which row
						_all_row[stod(rect2)]->_this_row_cells.insert({ temp_name,_tempcell });            // set this cell is on which row
						_all_cell.insert({temp_name, _tempcell});         
					}
					//cout << "cell's name : " << temp_name << endl;
				}

				fin >> temp2;     //"COMPONENTS"
			
			}        //END COMPONENTS
			else if (temp == "SPECIALNETS") {
				while (temp != "END") {
					fin >> temp;				// -   VDD  +        // -   VSS   +
					if (temp == "VDD" || temp == "VSS")      _which_pg = temp;
					while (temp == "ROUTED" || temp == "NEW") {
						vector<string> _PG_net_info;
						_PG_net_info.clear();
						_PG_net_info.push_back(_which_pg);           //  "VDD"  or "VSS"
						fin >> temp;			//Metal3
						_PG_net_info.push_back(temp);
						fin >> temp;			//400
						_PG_net_info.push_back(temp);
						while(temp!="(")     fin >> temp;			//+   SHAPE  STRIPE  (
						fin >> rect1;            //20000
						_PG_net_info.push_back(rect1);
						fin >> rect2;             //31400
						_PG_net_info.push_back(rect2);
						fin >> temp;		 fin >>temp;			// )  (
						fin >> temp;			//1800000    or    * 
						if(temp=="*")   _PG_net_info.push_back(rect1);
						else                   _PG_net_info.push_back(temp);
						fin >> temp;            // 1541600    or    *
						if (temp == "*")   _PG_net_info.push_back(rect2);
						else                   _PG_net_info.push_back(temp);
						fin >> temp;               // )

						_VDD_VSS.push_back(_PG_net_info);			//: VDD(VSS),  Metal3,  400,  20000,  20200,  1800000,  20200

						fin >> temp;       //NEW

					}				//while (temp == "ROUTED" || temp == "NEW")
				}
			}
		}			//finish fin

		//////////////////void sort each row's cell  ( according to _y)
		cout << "_units: " << _units << endl;
		sort_cell_on_row();

	}     //file is open
	cout << "finish parser def !!!" << endl;
}

///////////////////////////////////////////////////////////////////
/*void design::insert_VP_skew_initial(string which_cell_type)    //skew , up     // that type cell ,insert VP the same shape
{
	//pin* _out_pin;
	//int _which_pin = -1;
	//for (int i = 0; i < _all_cell_type_new[which_cell_type]->_all_pin.size(); ++i) {
	//	if (_all_cell_type_new[which_cell_type]->_all_pin.at(i)->getdir() == "OUTPUT") {
	//		_out_pin = _all_cell_type_new[which_cell_type]->_all_pin.at(i);
	//		break;
	//	}
	//}
	//_out_pin->_obs= _out_pin->get_RECT();
	//for (int i = 0; i < _out_pin->_obs.size(); ++i) {
	//	if (_all_layer[_out_pin->getlayer()]->get_direction() == 0) {     //pin在的那個metal (1or2) 是繞horizontal
	//		if (abs(_out_pin->_obs.at(i).first.first - _out_pin->_obs.at(i).second.first) > abs(_out_pin->_obs.at(i).first.second - _out_pin->_obs.at(i).second.second)) {   //x2-x2 > y1-y2    //是一個水平形狀的pin
	//			_which_pin = i;
	//			break;
	//		}
	//	}
	//	else {									//pin在的那個metal (1or2) 是繞vertical
	//		if (abs(_out_pin->_obs.at(i).first.second - _out_pin->_obs.at(i).second.second) > abs(_out_pin->_obs.at(i).first.first - _out_pin->_obs.at(i).second.first)) {   // y1-y2 > x2-x2   //是一個垂直線形狀的pin
	//			_which_pin = i;
	//			break;
	//		}

	//	}
	//}
	//
	////已知道要在第i個pin上打上面那曾layer的VP				//_out_pin->_obs.at(_which_pin)     , is a  pair<pair<double, double>, pair<double, double>>
	////map<string, layer*>::iterator
	//auto iter = _all_layer.find(_out_pin->getlayer());
	//iter++;
	//for (; iter != _all_layer.end(); iter++) {
	//	_out_pin->_obs.at(_which_pin).first.first;
	//	//////////////////////////////////////////////////////////斜的要怎麼劃分每一層的相鄰兩metal......../////////////////////////////
	//}
	////寫一些東西到lef檔
}*/
	
vector<RECT>design::BuildVP(int dir,  vector<RECT> &pre_vp, double _wire_width,int _slice) {
	double XRange[2] = { DBL_MAX,DBL_MIN };
	double YRange[2] = { DBL_MAX,DBL_MIN };
	double min_XRange = 0.5;
	double min_YRange = 0.5;
	double VP_X[2] = { 0 };
	double VP_Y[2] = { 0 };
	vector<RECT> ans;

	for (int i = 0; i < pre_vp.size(); ++i) { //找出最小x 最小y
		if (pre_vp[i].Point(1,'x') < XRange[0]) {
			XRange[0] = pre_vp[i].Point(1, 'x'); //X1
		}
		if (pre_vp[i].Point(2, 'x') < XRange[0]) {
			XRange[0] = pre_vp[i].Point(2, 'x');//X2
		}

		if (pre_vp[i].Point(1, 'y') < YRange[0]) {
			YRange[0] = pre_vp[i].Point(1, 'y'); //Y1
		}
		if (pre_vp[i].Point(2, 'y') < YRange[0]) {
			YRange[0] = pre_vp[i].Point(2, 'y');//Y2
		}
	}

	for (int i = 0; i < pre_vp.size(); ++i) { //找出最大x 最大y
		if (pre_vp[i].Point(1, 'x') > XRange[1]) {
			XRange[1] = pre_vp[i].Point(1, 'x'); //X1
		}
		if (pre_vp[i].Point(2, 'x') > XRange[1]) {
			XRange[1] = pre_vp[i].Point(2, 'x');//X2
		}

		if (pre_vp[i].Point(1, 'y') > YRange[1]) {
			YRange[1] = pre_vp[i].Point(1, 'y'); //Y1
		}
		if (pre_vp[i].Point(2, 'y') > YRange[1]) {
			YRange[1] = pre_vp[i].Point(2, 'y');//Y2
		}
	}

	if ((XRange[1] - XRange[0] < min_XRange) && (YRange[1] - YRange[0] < min_YRange)) {      //第一層上方 (打第二層時) : 不夠長也不夠寬
		if (dir == 1) {               //第一層是水平走向，第二層是垂直走向
			double t = (min_YRange - (YRange[1] - YRange[0])) / 2;        //只有y能延長 (x延長的話就會碰不到第一層的pin (開路)
			YRange[0] -= t;
			YRange[1] += t;
		}
		else {				//第一層是垂直走向，第二層是水平走向
			double t = (min_XRange - (XRange[1] - XRange[0])) / 2;		//只有x能延長 (y延長的話就會碰不到第一層的pin (開路)
			XRange[0] -= t;
			XRange[1] += t;
		}
	}
	else {      //第二層以上
		if (XRange[1] - XRange[0] < min_XRange) {                                    //第一層 目標pin上 可能會太短-->變長
			double t = (min_XRange - (XRange[1] - XRange[0])) / 2;
			XRange[0] -= t;
			XRange[1] += t;
		}

		if (YRange[1] - YRange[0] < min_YRange) {						//第一層 目標pin上 可能會太短-->變長
			double t = (min_YRange - (YRange[1] - YRange[0])) / 2;
			YRange[0] -= t;
			YRange[1] += t;
		}
	}

	double w= _wire_width; //線寬
	double _first_VP_Y_0 = 0, _first_VP_Y_1 = 0;
	double _first_VP_X_0 = 0, _first_VP_X_1 = 0;
	if (dir == 0) {				//dir == 'H'                //打新的VP  左下右上點
		double x_buffer = 0.05/4; //x超出多少?
		double y_buffer = 0.05/4; //y縮進去多少?

		VP_X[0]=XRange[0] - x_buffer;                          // VP1's X
		VP_X[1] =XRange[1] + x_buffer;

		VP_Y[0] = YRange[0] + y_buffer;
		VP_Y[1] = YRange[0] + y_buffer+ w;				// VP1's Y

		_first_VP_Y_0 = VP_Y[0];					// if _slice=3 , need this
		_first_VP_Y_1 = VP_Y[1];

		RECT _temp_RECT;
		_temp_RECT.Point(1, 'x') = VP_X[0];
		_temp_RECT.Point(1, 'y') = VP_Y[0];
		_temp_RECT.Point(2, 'x') = VP_X[1];
		_temp_RECT.Point(2, 'y') = VP_Y[1];
		ans.push_back(_temp_RECT);


		VP_Y[0] = YRange[1] - y_buffer - w;                             // VP2's Y       (VP2和VP1的X一樣)
		VP_Y[1] = YRange[1] - y_buffer;
		_temp_RECT.Point(1, 'y') = VP_Y[0];
		_temp_RECT.Point(2, 'y') = VP_Y[1];
		ans.push_back(_temp_RECT);

		if (_slice == 3) {
			//new VP_Y[0] : (_first_VP_Y_0+ VP_Y[0])/2
			//new VP_Y[1] : (_first_VP_Y_1 + VP_Y[1])/2
			_temp_RECT.Point(1, 'y') = (_first_VP_Y_0 + VP_Y[0]) / 2;
			_temp_RECT.Point(2, 'y') = (_first_VP_Y_1 + VP_Y[1]) / 2;
			ans.push_back(_temp_RECT);
		}

	}
	else if(dir==1) {			//dir=='V'
		double x_buffer = 0.05/4; //x縮進去多少?
		double y_buffer = 0.05/4; //y超出多少?

		VP_X[0] = XRange[0] + x_buffer;						// VP1's X
		VP_X[1] = XRange[0] + x_buffer + w;

		_first_VP_X_0 = VP_X[0];								// if _slice=3 , need this
		_first_VP_X_1 = VP_X[1];

		VP_Y[0] = YRange[0] - y_buffer;						// VP1's Y
		VP_Y[1] = YRange[1] + y_buffer;

		RECT _temp_RECT;
		_temp_RECT.Point(1, 'x') = VP_X[0];
		_temp_RECT.Point(1, 'y') = VP_Y[0];
		_temp_RECT.Point(2, 'x') = VP_X[1];
		_temp_RECT.Point(2, 'y') = VP_Y[1];
		ans.push_back(_temp_RECT);

		VP_X[0] = XRange[1] - x_buffer - w;					// VP2's X    (VP2和VP1的Y一樣)
		VP_X[1] = XRange[1] - x_buffer;

		_temp_RECT.Point(1, 'x') = VP_X[0];
		_temp_RECT.Point(2, 'x') = VP_X[1];
		ans.push_back(_temp_RECT);

		if (_slice == 3) {
			//new VP_X[0] : (_first_VP_X_0+ VP_X[0])/2
			//new VP_X[1] : (_first_VP_X_1 + VP_X[1])/2
			_temp_RECT.Point(1, 'x') = (_first_VP_X_0 + VP_X[0]) / 2;
			_temp_RECT.Point(2, 'x') = (_first_VP_X_1 + VP_X[1]) / 2;
			ans.push_back(_temp_RECT);
		}
	}
	else {
		//err
		cout << "Err in \"design.h\": " << __LINE__ << endl;
	}
	return ans;
}

void design::insert_VP_straight_initial(string which_cell_type)
{

	pin* _out_pin;
	_out_pin = NULL;
	int _which_pin = -1;
	int base_dir = -1;
	int each_dir = -1;
	int flag = 0;

	cout << "start " << which_cell_type << " !" << endl;

	for (int i = 0; i < _all_cell_type_new[which_cell_type]->_all_pin.size(); ++i) {
		if (_all_cell_type_new[which_cell_type]->_all_pin.at(i)->getdir() == "OUTPUT") {
			_out_pin = _all_cell_type_new[which_cell_type]->_all_pin.at(i);
			break;
		}
	}

	double _suitable_height = 0;
	if (_out_pin != NULL) {
		for (int i = 0; i < _out_pin->Get_rect().size(); ++i) {
			if (_all_layer[_out_pin->getlayer().at(0)]->get_direction() == 0) {     //pin在的那個metal (1or2) 是繞horizontal
				/*if (abs(_out_pin->get_RECT().at(i).first.first - _out_pin->get_RECT().at(i).second.first) > abs(_out_pin->get_RECT().at(i).first.second - _out_pin->get_RECT().at(i).second.second)) {   //x2-x2 > y1-y2    //是一個水平形狀的pin
					if (abs(_out_pin->get_RECT().at(i).first.first - _out_pin->get_RECT().at(i).second.first) > 0.4) {              //變> 長的1/3
						_which_pin = i;
						base_dir = 0;         //此pin的一個_which_pin線段是水平走向
						cout << "There is a horizontal dir on metal 1 " << endl;
						cout << "flag=1   !!!!!!!!!!!" << endl;
						flag = 1;
						break;
					}
				}					*/
				flag = 0;

			}
			else {									//pin在的那個metal (1or2) 是繞vertical
				if (abs(_out_pin->Get_rect().at(i).Point(1,'y') - _out_pin->Get_rect().at(i).Point(2,'y')) > abs(_out_pin->Get_rect().at(i).Point(1,'x') - _out_pin->Get_rect().at(i).Point(2,'x'))) {   // y1-y2 > x2-x2   //是一個垂直線形狀的pin
					if (abs(_out_pin->Get_rect().at(i).Point(1, 'y') - _out_pin->Get_rect().at(i).Point(2, 'y')) > 0.4) {
						_which_pin = i;
						base_dir = 1;         //此pin的一個_which_pin線段是垂直走向
						cout << "There is a vertical dir on metal 1 " << endl;
						cout << "flag=1   !!!!!!!!!!!" << endl;
						cout << "_which_pin : " << _which_pin << endl;
						flag = 1;
						break;
					}

				}
			}
		}

	/*	flag = 0;
		base_dir = 0;*/

		vector<RECT> pre_vp;
		vector<RECT> now_vp;
		RECT _temp_rect;

		double _max_height = 0;
		double _spacing = 0.5;

		auto iter = _all_layer.find(_out_pin->getlayer().at(0));			//metal1					////////////////////////////////////////////////////////////////////

		if (flag == 0) {					//沒有同方向的output pin: 找一個直的, 最長的output pin ( metal1 is horizontal)
			base_dir = iter->second->get_direction();							//( metal1 is horizontal)
			if (base_dir == 0){
				for (int j = 0; j < _out_pin->Get_rect_size(); ++j) {
					if (abs(_out_pin->Get_rect().at(j).Point(2,'y')- _out_pin->Get_rect().at(j).Point(1,'y')) > _max_height) {
						_max_height = abs(_out_pin->Get_rect().at(j).Point(2, 'y') - _out_pin->Get_rect().at(j).Point(1, 'y'));
						cout << "vertical one on horizontal metal1!!" << endl;
						_which_pin = j;

					}
				}

			//往右或往左一步 再打一條直的 (metal 2)
			pre_vp.clear();
			pre_vp.push_back(_out_pin->Get_rect().at(_which_pin));                     //metal2 第一根
			/*if (_out_pin->get_RECT().at(_which_pin).first.first + _spacing > _all_cell_type_new[which_cell_type]->get_size()) {			//往左一步
				_temp_rect.first.first = _out_pin->get_RECT().at(_which_pin).first.first - _spacing;
				_temp_rect.first.second = _out_pin->get_RECT().at(_which_pin).first.second;
				_temp_rect.second.first = _out_pin->get_RECT().at(_which_pin).second.first - _spacing;
				_temp_rect.second.second = _out_pin->get_RECT().at(_which_pin).second.second;
			}
			else {
				_temp_rect.first.first = _out_pin->get_RECT().at(_which_pin).first.first + _spacing;                                       //往右一步   (大部分都長這樣)
				_temp_rect.first.second = _out_pin->get_RECT().at(_which_pin).first.second;
				_temp_rect.second.first = _out_pin->get_RECT().at(_which_pin).second.first + _spacing;
				_temp_rect.second.second = _out_pin->get_RECT().at(_which_pin).second.second;
			}*/

			cout << "this_cell_type's width : " << _all_cell_type_new[which_cell_type]->get_size() << endl;

			if (_out_pin->Get_rect().at(_which_pin).Point(1, 'x') - _spacing < (0+0.15)) {			//往右一步
				_temp_rect.Point(1, 'x') = _out_pin->Get_rect().at(_which_pin).Point(1, 'x') +_spacing;
				_temp_rect.Point(1, 'y') = _out_pin->Get_rect().at(_which_pin).Point(1, 'y');
				_temp_rect.Point(2, 'x') = _out_pin->Get_rect().at(_which_pin).Point(2, 'x') + _spacing;
				_temp_rect.Point(2, 'y') = _out_pin->Get_rect().at(_which_pin).Point(2, 'y');
			}
			else {
				_temp_rect.Point(1, 'x') = _out_pin->Get_rect().at(_which_pin).Point(1, 'x') - _spacing;                                       //往左一步  (大部分都長這樣)
				_temp_rect.Point(1, 'y') = _out_pin->Get_rect().at(_which_pin).Point(1, 'y');
				_temp_rect.Point(2, 'x') = _out_pin->Get_rect().at(_which_pin).Point(2, 'x') - _spacing;
				_temp_rect.Point(2, 'y') = _out_pin->Get_rect().at(_which_pin).Point(2, 'y');
			}
			


			/* if (_out_pin->get_RECT().at(_which_pin).first.first - _spacing < (0 + 0.15)) {			//往右一步
				_temp_rect.first.first = _out_pin->get_RECT().at(_which_pin).first.first - _spacing;
				_temp_rect.first.second = _out_pin->get_RECT().at(_which_pin).first.second;
				_temp_rect.second.first = _out_pin->get_RECT().at(_which_pin).second.first - _spacing;
				_temp_rect.second.second = _out_pin->get_RECT().at(_which_pin).second.second;
			}
			else {
				_temp_rect.first.first = _out_pin->get_RECT().at(_which_pin).first.first + _spacing;                                       //往左一步  (大部分都長這樣)
				_temp_rect.first.second = _out_pin->get_RECT().at(_which_pin).first.second;
				_temp_rect.second.first = _out_pin->get_RECT().at(_which_pin).second.first + _spacing;
				_temp_rect.second.second = _out_pin->get_RECT().at(_which_pin).second.second;
			}*/

				pre_vp.push_back(_temp_rect);       //metal2 第二根
		}

			else if (base_dir == 1) {                ////////////////比較少用，盡量不要用
				for (int j = 0; j < _out_pin->Get_rect_size(); ++j) {
					if (abs(_out_pin->Get_rect().at(j).Point(1, 'x') - _out_pin->Get_rect().at(j).Point(2, 'x')) > _max_height) {
						_max_height = abs(_out_pin->Get_rect().at(j).Point(1, 'x') - _out_pin->Get_rect().at(j).Point(2, 'x'));
						cout << "horizontal one on vertical metal1!!" << endl;
						_which_pin = j;

					}
				}


				//往下一步 再打一條橫的 (metal 2)
				pre_vp.clear();
				pre_vp.push_back(_out_pin->Get_rect().at(_which_pin));                     //metal2 第一根
				_temp_rect.Point(1, 'x') = _out_pin->Get_rect().at(_which_pin).Point(1, 'x');
				_temp_rect.Point(1, 'y') = _out_pin->Get_rect().at(_which_pin).Point(1, 'y') + _spacing;
				_temp_rect.Point(2, 'x') = _out_pin->Get_rect().at(_which_pin).Point(2, 'x');
				_temp_rect.Point(2, 'y') = _out_pin->Get_rect().at(_which_pin).Point(2, 'y') +_spacing;
				pre_vp.push_back(_temp_rect);       //metal2 第二根

			}						  ////////////////比較少用，盡量不要用

			for (int i = 0; i < _out_pin->Get_rect_size(); ++i) {				//_out_pin的_RECT全部給_obs
				_out_pin->_obs.push_back(_out_pin->Get_rect().at(i));
				_out_pin->_obs_layer.push_back(_out_pin->getlayer().at(i));
			}
			_out_pin->Clear_rect();

			auto iter = _all_layer.find(_out_pin->getlayer().at(0));			//metal1
			_out_pin->_clear_layer();
			iter++;			//metal2
			each_dir = (base_dir == 1) ? 0 : 1;				//下一層走向更新 (metal2)

			if (each_dir == 1) {                    //如果metal2是vertical direction的這種case
				cout << "pre_vp.at(0) : " << pre_vp.at(0).Point(1, 'y') << "   " << pre_vp.at(0).Point(2, 'y') << "    " << pre_vp.at(1).Point(1, 'y') << "      " << pre_vp.at(1).Point(2, 'y') << endl;
				//offset(pre_vp, iter->second->get_spacing() + 0.1, 0.15, 1.05);
				cout << "pre_vp.at(0) : " << pre_vp.at(0).Point(1, 'y') << "   " << pre_vp.at(0).Point(2, 'y') << "    " << pre_vp.at(1).Point(1, 'y') << "      " << pre_vp.at(1).Point(2, 'y') << endl;
			}

			_out_pin->_obs.insert(_out_pin->_obs.end(), pre_vp.begin(), pre_vp.end());
			_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
			_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer

			iter++;			//metal3
			each_dir = (each_dir == 1) ? 0 : 1;				//下一層走向更新 (metal3)


			now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(),3);    //metal3 特別處理
			_out_pin->_obs.insert(_out_pin->_obs.end(), now_vp.begin(), now_vp.end());
			_out_pin->_obs_layer.push_back(iter->first);       // 三根新加入的obs所在的metal layer
			_out_pin->_obs_layer.push_back(iter->first);       // 三根新加入的obs所在的metal layer
			_out_pin->_obs_layer.push_back(iter->first);       // 三根新加入的obs所在的metal layer
			pre_vp.clear();
			pre_vp = now_vp;
			now_vp.clear();
			each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新
			iter++;			//metal4

			for (; iter != --(--_all_layer.end()); iter++) {            //第"四"層~倒數第三層
				now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(),2);
				_out_pin->_obs.insert(_out_pin->_obs.end(), now_vp.begin(), now_vp.end());
				_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
				_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
				pre_vp.clear();
				pre_vp = now_vp;
				now_vp.clear();
				each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新
			}
			//now_VP+= _all_cell_type_new[which_cell_type]->_this_type_all_cell.size();
			now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(),2);		//最後一根 : 倒數第二層
			_out_pin->Set_rect(now_vp.at(0));
			_out_pin->setlayer(iter->first);

		}



		else if (flag == 1) {						//有和metal1同方向的output pin
			for (int i = 0; i < _out_pin->Get_rect_size(); ++i) {				//_out_pin的_RECT全部給_obs
				_out_pin->_obs.push_back(_out_pin->Get_rect().at(i));
				_out_pin->_obs_layer.push_back(_out_pin->getlayer().at(i));
			}
			_out_pin->Clear_rect();

			//已知道要在第_which_pin個pin上打上面那曾layer的VP				
			auto iter = _all_layer.find(_out_pin->getlayer().at(0));
			_out_pin->_clear_layer();
			iter++;

			vector<RECT> pre_vp;
			vector<RECT> now_vp;
			each_dir = (base_dir == 1) ? 0 : 1;				//下一層走向更新
			pre_vp.clear();
			pre_vp.push_back(_out_pin->_obs.at(_which_pin));      //第一根

			now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(),2);				// metal2 特別處理
			//為防止這兩根直的(或衡的 但較少見) VP在metal2會和VSS VDD短路
			if (each_dir == 1) {                    //如果metal2是vertical direction的這種case
				cout << "now_vp.at(0) : " << now_vp.at(0).Point(1,'y') << "   " << now_vp.at(0).Point(2,'y') << "    " << now_vp.at(1).Point(1, 'y') << "      " << now_vp.at(1).Point(2, 'y') << endl;
				//offset(now_vp, iter->second->get_spacing() + 0.1, 0.15, 1.05);
			}

			cout << "now_vp.at(0) : " << now_vp.at(0).Point(1, 'y') << "   " << now_vp.at(0).Point(2, 'y') << "    " << now_vp.at(1).Point(1, 'y') << "      " << now_vp.at(1).Point(2, 'y') << endl;
			_out_pin->_obs.insert(_out_pin->_obs.end(), now_vp.begin(), now_vp.end());
			_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
			_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
			pre_vp.clear();
			pre_vp = now_vp;
			now_vp.clear();
			each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新 (metal3)
			iter++;                   //metal3

			now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(), 3);				// metal3 特別處理
			_out_pin->_obs.insert(_out_pin->_obs.end(), now_vp.begin(), now_vp.end());
			_out_pin->_obs_layer.push_back(iter->first);       // 三根新加入的obs所在的metal layer
			_out_pin->_obs_layer.push_back(iter->first);       // 三根新加入的obs所在的metal layer
			_out_pin->_obs_layer.push_back(iter->first);       // 三根新加入的obs所在的metal layer
			pre_vp.clear();
			pre_vp = now_vp;
			now_vp.clear();
			each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新 (metal4)
			iter++;                   //metal4

			now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(), 2);				//metal4特別處理
			if (each_dir == 1) {                    //如果metal4是vertical direction的這種case
				cout << "now_vp.at(0) metal4: " << now_vp.at(0).Point(1, 'y') << "   " << now_vp.at(0).Point(2, 'y') << "    " << now_vp.at(1).Point(1, 'y') << "      " << now_vp.at(1).Point(2, 'y') << endl;
				//offset(now_vp, iter->second->get_spacing()+0.1, 0.06, 1.14);
				
			}
			cout << "now_vp.at(0) metal4: " << now_vp.at(0).Point(1, 'y') << "   " << now_vp.at(0).Point(2, 'y') << "    " << now_vp.at(1).Point(1, 'y') << "      " << now_vp.at(1).Point(2, 'y') << endl;
			_out_pin->_obs.insert(_out_pin->_obs.end(), now_vp.begin(), now_vp.end());
			_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
			_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
			pre_vp.clear();
			pre_vp = now_vp;
			now_vp.clear();
			each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新  (metal5)
			iter++;                   //metal5

			for (; iter != --(--_all_layer.end()); iter++) {            //第五層~倒數第三層
				now_vp = BuildVP(each_dir, pre_vp,iter->second->get_width(),2);
				_out_pin->_obs.insert(_out_pin->_obs.end(), now_vp.begin(), now_vp.end());
				_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
				_out_pin->_obs_layer.push_back(iter->first);       // 兩根新加入的obs所在的metal layer
				pre_vp.clear();
				pre_vp = now_vp;
				now_vp.clear();
				each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新
			}
			//now_VP+= _all_cell_type_new[which_cell_type]->_this_type_all_cell.size();
			now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(),2);		//最後一根 : 倒數第二層
			_out_pin->Set_rect(now_vp.at(0));
			_out_pin->setlayer(iter->first);

			// 此pin的obs們變成一個新的pin
			/*cout << "t" + to_string(_pins_of_obs.size()) << endl;
			pin* _temp_pin = new pin("t"+ to_string(_pins_of_obs.size()), "INPUT", "SIGNAL");
			for (int i = 0; i < _out_pin->_obs.size(); ++i) {				 
				_temp_pin->set_RECT(_out_pin->_obs.at(i));					  // 此pin的obs們給去(當成)新的pin的RECT
				_temp_pin->setlayer(_out_pin->_obs_layer.at(i));			  // 此pin的obs layer 給去(當成)新的pin的layer
			}
			_pins_of_obs.insert({ "t" + to_string(_pins_of_obs.size()), _temp_pin });			*/

			//寫一些東西到lef檔

			/*cout << which_cell_type << endl;
			cout << "_out_pin name : " << _out_pin->getname() << endl << endl;
			cout << "LAYER " << _out_pin->getlayer()<< " ;" << endl;
			cout << "RECT " << _out_pin->get_first_RECT().first.first << " " <<
				_out_pin->get_first_RECT().first.second << " " <<
				_out_pin->get_first_RECT().second.first << " " <<
				_out_pin->get_first_RECT().second.second << " ;" << endl;

			string pre_layer, now_layer;
			pre_layer.clear();
			now_layer.clear();
			cout << "OBS" << endl;
			for (int i = 0; i < _out_pin->_obs.size(); ++i) {					//_obs size應該要等於_obs_layer size
				now_layer = _out_pin->_obs_layer.at(i);
				if (pre_layer != now_layer) {
					cout << "LAYER " << _out_pin->_obs_layer.at(i) << " ;" << endl;
				}
				cout << "RECT " << _out_pin->_obs.at(i).first.first << " " <<
					_out_pin->_obs.at(i).first.second << " " <<
					_out_pin->_obs.at(i).second.first << " " <<
					_out_pin->_obs.at(i).second.second << " ;" << endl;
				pre_layer = now_layer;
			}
			cout << "END" << endl;				*/
		}
	}
	else {
	cout << "output pin=NULL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	cout <<"_this_type_all_cell.size() : "<<_all_cell_type_new[which_cell_type]->_this_type_all_cell.size() << endl;
}
}

void design::insert_all_VP_initial()
{
	
	cout <<" _all_cell_type_new.size(): "<< _all_cell_type_new.size() << endl;
	for (const auto& s : _all_cell_type_new) {
		insert_VP_straight_initial(s.first);
	}
}


///
void design::insert_some_VP_initial(double percent)
{
	int now_insert_VP_num = 0;
	int now_insert_celltype_num = 0;
	int great_cell_type = 0;
	int great_cell = 0;
	double now_insert_VP_percent = 0;
	int target_VP_num = round(_components_num * percent);
	cout << "_components_num: " << _components_num << endl;
	cout << "_all_cell_type_new: " << _all_cell_type_new.size() << endl;
	cout << "target_num: " << target_VP_num << endl;
	
	for (const auto& s : _all_cell_type_new) {
		cout << "cell_type_name: " << s.first << "  :  " << s.second->get_size() << endl;
		if ((s.second->get_size() < 2)&& (s.second->get_size() > 0.9)) {
			great_cell_type ++;
			great_cell += s.second->_this_type_all_cell.size();

		}
	}




	cout << "great_cell_type: " << great_cell_type << endl;
	cout << "great_cell: " << great_cell << endl;
	now_insert_VP_percent = double(great_cell) / double(_components_num);
	cout << "_now_percent: " << now_insert_VP_percent << endl;
	if (now_insert_VP_percent > percent) {
		cout << "exceed! " <<endl;
	}
	else {
		cout << "do not exceed! " << endl;
		cout <<setprecision(2)<<fixed<< now_insert_VP_percent << endl;
	}  
		for (const auto& s : _all_cell_type_new) {
			if (now_insert_VP_num < target_VP_num) {
				if (  (s.second->get_size() < 1.8) && (s.second->get_size() > 0.9)) {
					insert_VP_straight_initial(s.first);
					now_insert_VP_num += s.second->_this_type_all_cell.size();
					cout << "s.second->_this_type_all_cell.size() : " << s.second->_this_type_all_cell.size() << endl;
					cout << "now_insert_VP_num : " << now_insert_VP_num << endl;
					now_insert_celltype_num++;
				}
			}
			else {
				break;
			}
		}
		cout << "final insert celltype num : " << now_insert_celltype_num << endl;
		cout << "final insert  VP num: " << now_insert_VP_num << endl;

}


///
void design::insert_dodge_VP(double percent)
{
	int target_VP_num = 0;
	int now_insert_VP_num = 0;
	target_VP_num = round(_components_num * percent);
	cout << "target_VP_num: " << target_VP_num << endl;
	cout << "_all_cell_type_new size: " << _all_cell_type_new.size() << endl;
	for (const auto &s : _all_cell_type_new) {
		if (now_insert_VP_num < target_VP_num) {
			if (  (s.second->get_size() < 2) && (s.second->get_size() >0.9) && (s.second->find_outpin() != NULL)) {                     
				cout << __LINE__ << endl;
				s.second->_has_insert_VP = 1;                                     // 這個種類的cell type 打了VP: _has_insert_VP=1
				for (int i = 0; i < s.second->_this_type_all_cell.size(); ++i) {
					cout << __LINE__ << endl;
				      dodge_pin_VP(s.second->_this_type_all_cell.at(i));
					cout << __LINE__ << endl;
					s.second->_this_type_all_cell.at(i)->_new_cell_type_name = s.first + "_" + to_string(i);
					s.second->_this_type_all_cell.at(i)->_VP_pin_name = "t"+s.second->find_outpin()->getname();
				}
				cout << "ADD: " <<s.first<<"    " <<s.second->_this_type_all_cell.size() << endl;
				now_insert_VP_num += s.second->_this_type_all_cell.size();
			}
		}
		else {
			break;
		}
	}
	
	cout << "final insert VP num: " << now_insert_VP_num << endl;
	cout << "final insert VP percent: "<<setprecision(3)<<fixed<<double(double(now_insert_VP_num) /double(_components_num))<<endl;


}


///
void design::dodge_pin_VP(cell* which_cell)
{
	int _M2_dir = _all_layer["Metal2"]->get_direction();
	cell_type* which_cell_type = which_cell->get_cell_type();             //this cell's celltype
	pin* output_pin = which_cell_type->find_outpin();      // little bit redundant

	double _x_coordinate = 0, _y_coordinate = 0;
	_x_coordinate = which_cell->get_x() / _units;
	_y_coordinate = which_cell->get_y() / _units;

	vector<RECT> pre_vp,now_vp,_m2_base;
	RECT _base_pin_rect;
	vector<box> _M4_overlap_PG;
	vector<value> _Result;
	char _M2VPbase_type;

	double _spacing = 0.5;
	int base_dir = -1;
	int each_dir = -1;
	base_dir = (_M2_dir == 1) ? 0 : 1;			//metal1 走向 (用metal2推)
	if (output_pin != NULL) {
		box b_cell_buffer(point( _x_coordinate-1,  _y_coordinate-1),
			point(_x_coordinate+which_cell_type->get_size()+1 ,_y_coordinate+which_cell_type->get_by()+1 ));
		which_cell->set_buffer_overlap_m2VP(oneMetal_VP_overlap("rtMetal2", which_cell->get_cell_name(), "", b_cell_buffer));
		which_cell->set_buffer_overlap_m3PG (oneMetal_VP_overlap("rtMetal3", which_cell->get_cell_name(), "", b_cell_buffer),0.1);
		which_cell->set_buffer_overlap_m4PG (oneMetal_VP_overlap("rtMetal4", which_cell->get_cell_name(), "", b_cell_buffer),0.1);

		// which_cell->reset_this_pin();
		which_cell->get_cell_type()->find_outpin()->_reset_all_RECT_access();

		while (_m2_base.empty()) {                 // 如果沒有合法的M2第一根VP, 跑此while
			_base_pin_rect = choose_base_pin_rect(which_cell_type, _M2_dir);                 //VP M2第一根 和 M2_dir方向一致
			if (_base_pin_rect.get_access_flag() == -1) {  //output pin 中沒有_M2_dir的RECT了 
				_base_pin_rect = choose_base_pin_rect(which_cell_type, (_M2_dir == 1) ? 0 : 1);        //VP M2第一根 和 M2_dir方向不同
			}
			if (_base_pin_rect.get_access_flag() == -1) {                      // 還是-1，代表所有rect都被找完了 
				//////////////////////////pre_vp 要 push_back()  一個和_M2_dir同方向的 pin base////////////////////////
				_base_pin_rect=choose_specify_dir_pin_rect(which_cell_type, _M2_dir);
				_m2_base.push_back(_base_pin_rect);
				_M2VPbase_type = 'A';
				break;
			}
			int flag_overlap_with_PG = 0;
			if (_base_pin_rect.horizontal_or_straight() == _M2_dir) {              //VP M2第一根(base pin) 和 M2_dir方向一致

					   // 看M2第一根pin有沒有和 上方任何PG重疊
				flag_overlap_with_PG=check_overlap(_base_pin_rect.RECT2box(_x_coordinate, _y_coordinate), which_cell->get_buffer_overlap_m3PG(), _Result);     //M3 PG
				flag_overlap_with_PG= check_overlap(_base_pin_rect.RECT2box(_x_coordinate, _y_coordinate), which_cell->get_buffer_overlap_m4PG(), _Result);     //M4 PG
				if (flag_overlap_with_PG == 0) {                                //M2第一根VP沒有和上方PG重疊
					_m2_base.push_back(_base_pin_rect);
					_M2VPbase_type = 'A';
				}
			}
			else {                            //VP M2 pin base 和 M2_dir方向不同
				int flag_overlap_with_PG = 0;
				// 看M2 pin base有沒有和 上方任何PG重疊
				// M4 PG 就算有和_base_pin_rect 重疊也可選 ( M2 VP兩根 的走向和M4PG的相同)
				_Result.clear();
				if (check_overlap(_base_pin_rect.RECT2box(_x_coordinate, _y_coordinate), which_cell->get_buffer_overlap_m4PG(), _Result) == 1) {  //M4 PG
					flag_overlap_with_PG = -1;
					for (int i = 0; i < _Result.size(); ++i) {
						_M4_overlap_PG.push_back(_Result.at(i).first);
					}
				}
				
				flag_overlap_with_PG = check_overlap(_base_pin_rect.RECT2box(_x_coordinate, _y_coordinate), which_cell->get_buffer_overlap_m3PG(), _Result);  //M3PG

				if (flag_overlap_with_PG == 0) {                                //M2 pin base沒有和上方PG重疊
					_m2_base.push_back(_base_pin_rect);
					_M2VPbase_type = 'C';
				}
				else if (flag_overlap_with_PG == -1) {                    //M2 pin base和上方M4 PG重疊， 但他們走向呈十字 ，可選，可閃避的掉
					_m2_base.push_back(_base_pin_rect);
					_M2VPbase_type = 'B';
				}

			}                       //VP M2 pin base 和 M2_dir方向不同結束
		}               //while (pre_vp.empty()
		cout << __LINE__ <<"   END while (pre_vp.empty())"<< endl;

		RECT _temp_rect, _temp_rect2;
		//////有pin base了 要開始蓋VP了
		if (_M2VPbase_type == 'A') {
			pre_vp.push_back(_m2_base.at(0));                  //M2第一根
			_temp_rect2 = _m2_base.at(0);                        //M2第二根=M2第一根，然後再開始shift

			int _is_left = 0, _is_up = 0, _has_overlap_V = 0, _has_overlap_H = 0;

			if (_m2_base.at(0).horizontal_or_straight() == 1) {              //M2第一根 is V
				if (_m2_base.at(0).Point(1, 'x') - _spacing < (0 + 0.15)) {			//往右一步打M2第二根
					_temp_rect2.Shift(_spacing, 0);
					_is_left = 0;
				}
				else {                                   //往左一步打M2第二根   (大部分都長這樣)
					_temp_rect2.Shift(-1 * _spacing, 0);
					_is_left = 1;
				}

				_Result.clear();
				if (check_overlap(_temp_rect2.RECT2box(_x_coordinate, _y_coordinate), which_cell->get_buffer_overlap_m4PG(), _Result)) {    //M2第二根和M4PG check overlap
					if (_is_left == 0) {
						_temp_rect2.Shift(-2 * _spacing, 0);
						_is_left = 1;
					}
					else {     //_is_left==1
						_temp_rect2.Shift(2 * _spacing, 0);
						_is_left = 0;
					}
				}


				////////////////_temp_rect2跟M1 pin比較看有沒有overlap
				for (int i = 0; i < which_cell_type->_all_pin.size(); ++i) {
					if (which_cell_type->_all_pin.at(i) != which_cell_type->find_outpin()) {
						for (int j = 0; j < which_cell_type->_all_pin.at(i)->Get_rect_size(); ++j) {
							_has_overlap_V = bg::intersects(_temp_rect2.RECT2box(), which_cell_type->_all_pin.at(i)->Get_rect().at(j).RECT2box());
							if (_has_overlap_V == 1)   break;
						}
						if (_has_overlap_V == 1)   break;
					}
				}

				if (_has_overlap_V) {              //has overlap
					if (_is_left == 0) {
						_temp_rect2.Shift(0.13, 0);
					}
					else {          //_is_left==1
						_temp_rect2.Shift(-0.13, 0);
					}
				}

			pre_vp.push_back(_temp_rect2);
				
			}    //M2第一根 is V   finish
			else {                   //M2第一根 is H  

				if (_m2_base.at(0).Point(1, 'y') - _spacing <0) {			//往下一步打M2第二根  (打出cell下界)
					_temp_rect2.Shift(0, -1*_spacing);
					_is_up = 0;
				}
				else {                                   //往上一步打M2第二根   (打出cell上界)
					_temp_rect2.Shift(0,1 * _spacing);
					_is_up = 1;
				}

				_Result.clear();
				if (check_overlap(_temp_rect2.RECT2box(_x_coordinate, _y_coordinate), which_cell->get_buffer_overlap_m4PG(), _Result)) {    //M2第二根和M4PG check overlap
					if (_is_up == 0) {
						_temp_rect2.Shift(0, 2 * _spacing);
						_is_up = 1;
					}
					else {     //_is_up==1
						_temp_rect2.Shift(0, -2 * _spacing);
						_is_up = 0;
					}
				}


				////////////////_temp_rect2應該要跟M2 VP比較看有沒有overlap， 但是還想不到如果有overlap的話要怎麼移動或改動
			/*	for (int i = 0; i < which_cell_type->_all_pin.size(); ++i) {
					if (which_cell_type->_all_pin.at(i) != which_cell_type->find_outpin()) {
						for (int j = 0; j < which_cell_type->_all_pin.at(i)->Get_rect_size(); ++j) {
							_has_overlap_V = bg::overlaps(_temp_rect2.RECT2box(), which_cell_type->_all_pin.at(i)->Get_rect().at(j).RECT2box());
							if (_has_overlap_V == 1)   break;
						}
						if (_has_overlap_V == 1)   break;
					}
				}*/

				//if (_has_overlap_H) {              //has overlap
				//	if (_is_up == 0) {
				//		_temp_rect2.Shift(0.13, 0);
				//	}
				//	else {          //_is_left==1
				//		_temp_rect2.Shift(-0.13, 0);
				//	}
				//}

				pre_vp.push_back(_temp_rect2);
			}        //M2第一根 is H  finish

		}
		else if (_M2VPbase_type == 'B') {
			vector<box>OverlapPG = _M4_overlap_PG;
			vector<RECT> base = _m2_base;

			if (_m2_base.at(0).horizontal_or_straight() == 0) { // base pin is H
			 // Rotate 90
				OverlapPG.clear();
				for (int i = 0; i < OverlapPG.size(); ++i) {
					OverlapPG.push_back(
						box(
							point(_M4_overlap_PG.at(i).min_corner().get < 1 >(), _M4_overlap_PG.at(i).min_corner().get < 0 >()),
							point(_M4_overlap_PG.at(i).max_corner().get < 1 >(), _M4_overlap_PG.at(i).max_corner().get < 0 >())
						)
					);
				}

				base.at(0).Point(1, 'x') = _m2_base.at(0).Point(1, 'y');                     //x y 顛倒
				base.at(0).Point(1, 'y') = _m2_base.at(0).Point(1, 'x');
				base.at(0).Point(2, 'x') = _m2_base.at(0).Point(2, 'y');
				base.at(0).Point(2, 'y') = _m2_base.at(0).Point(2, 'x');

			}


			double _m4PG_min_y = 1000000, _m4PG_max_y = 0;
			for (int i = 0; i < OverlapPG.size(); ++i) {
				if (OverlapPG.at(i).min_corner().get<1>() < _m4PG_min_y)   _m4PG_min_y = OverlapPG.at(i).min_corner().get<1>();
				if (OverlapPG.at(i).max_corner().get<1>() > _m4PG_max_y)    _m4PG_max_y = OverlapPG.at(i).max_corner().get<1>();
			}

			if (base.at(0).Point(1, 'y') >= _m4PG_min_y) {               //case B_1
				_temp_rect = RECT( point (base.at(0).Point(1, 'x') - _spacing / 2, _m4PG_max_y + 0.1), _spacing, 0.08);    //M2第一根
				_temp_rect2 = _temp_rect;
				_temp_rect2.Shift(0, _spacing);     //M2第二根
			}
			else if (base.at(0).Point(2, 'y') <= _m4PG_min_y) {              //case B_2
				_temp_rect = RECT(point(base.at(0).Point(1, 'x') - _spacing / 2, _m4PG_min_y - 0.1 - 0.08), _spacing, 0.08);    //M2第一根
				_temp_rect2 = _temp_rect;
				_temp_rect2.Shift(0, -1 * _spacing); //M2第二根
			}
			else {                                                 //case B_3
				_temp_rect = RECT(point(base.at(0).Point(1, 'x') - _spacing / 2, _m4PG_max_y + 0.1), _spacing,0.08);   //M2第一根
				_temp_rect2 = RECT(point(base.at(0).Point(1, 'x') - _spacing / 2, _m4PG_min_y - 0.1 - 0.08), _spacing, 0.08);   //M2第二根
			}

			if (_m2_base.at(0).horizontal_or_straight() == 0) { // base pin is H
				RECT _temp_rect3 = _temp_rect;
				_temp_rect.Point(1, 'x') = _temp_rect3.Point(1, 'y');
				_temp_rect.Point(1, 'y') = _temp_rect3.Point(1, 'x');
				_temp_rect.Point(2, 'x') = _temp_rect3.Point(2, 'y');
				_temp_rect.Point(2, 'y') = _temp_rect3.Point(2, 'x');

				_temp_rect3 = _temp_rect2;
				_temp_rect2.Point(1, 'x') = _temp_rect3.Point(1, 'y');
				_temp_rect2.Point(1, 'y') = _temp_rect3.Point(1, 'x');
				_temp_rect2.Point(2, 'x') = _temp_rect3.Point(2, 'y');
				_temp_rect2.Point(2, 'y') = _temp_rect3.Point(2, 'x');
			}     // Rotate -90

				pre_vp.push_back(_temp_rect);
			     pre_vp.push_back(_temp_rect2);
		}
		else if (_M2VPbase_type == 'C') {
			if (_m2_base.at(0).horizontal_or_straight() == 0) { // base pin is H
				_temp_rect = RECT(point((_m2_base.at(0).Point(1, 'x') + _m2_base.at(0).Point(2, 'x')) / 2, _m2_base.at(0).Point(2, 'y') - _spacing / 2), 0.08, _spacing);            //M2第一根  (middle)
				_Result.clear();
				check_overlap(_temp_rect.RECT2box(_x_coordinate,_y_coordinate), which_cell->get_buffer_overlap_m2VP(),_Result);    //check M2第一根有沒有和  M2VP, M3PG重疊
				check_overlap(_temp_rect.RECT2box(_x_coordinate, _y_coordinate), which_cell->get_buffer_overlap_m3PG(), _Result);    //check M2第一根有沒有和  M2VP, M3PG重疊
			       
				_temp_rect2 = _temp_rect;
				_temp_rect2.Shift( _spacing, 0);


			}
			else {           // base pin is V
				_temp_rect = RECT(point(_m2_base.at(0).Point(2, 'x') - _spacing / 2, _m2_base.at(0).Point(1, 'y') + 0.15) , _spacing, 0.08);      //M2第一根  (down)

				_temp_rect2 = _temp_rect;
				_temp_rect2.Shift(0,_spacing);
			}

			pre_vp.push_back(_temp_rect);
			pre_vp.push_back(_temp_rect2);

	}  //_M2VPbase_type=='C'  finish

	     //pre_vp.at(0),  pre_vp.at(1) 是做好的M2 2根VP  ，要加入M2rTree
	     add_rtree("Metal2", which_cell->get_cell_name(), output_pin->getname(), pre_vp.at(0).RECT2box(_x_coordinate, _y_coordinate));
	     add_rtree("Metal2", which_cell->get_cell_name(), output_pin->getname(), pre_vp.at(1).RECT2box(_x_coordinate, _y_coordinate));
		

		
		////////////////////////////接著metal3////////////////////////////

		auto iter = _all_layer.find(output_pin->getlayer().at(0));			//metal1
		iter++;			//metal2
		each_dir = (base_dir == 1) ? 0 : 1;				//下一層走向更新 (metal2)
		which_cell->_VP.insert(which_cell->_VP.end(), pre_vp.begin(), pre_vp.end());
		which_cell->_VP_layer.push_back(iter->first);
		which_cell->_VP_layer.push_back(iter->first);
		////////////////////////////接著metal3////////////////////////////
		iter++;			//metal3
		each_dir = (each_dir == 1) ? 0 : 1;				//下一層走向更新 (metal3)
		now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(), 3);    //metal3 特別處理
		which_cell->_VP.insert(which_cell->_VP.end(), now_vp.begin(), now_vp.end());
		which_cell->_VP_layer.push_back(iter->first);       // 三根新加入的VP所在的metal layer
		which_cell->_VP_layer.push_back(iter->first);       // 三根新加入的VP所在的metal layer
		which_cell->_VP_layer.push_back(iter->first);       // 三根新加入的VP所在的metal layer
		pre_vp.clear();
		pre_vp = now_vp;
		now_vp.clear();
		each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新
		iter++;			//metal4

		for (; iter != --(--_all_layer.end()); iter++) {            //第"四"層~倒數第三層
			now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(), 2);
			which_cell->_VP.insert(which_cell->_VP.end(), now_vp.begin(), now_vp.end());
			which_cell->_VP_layer.push_back(iter->first);       // 兩根新加入的VP所在的metal layer
			which_cell->_VP_layer.push_back(iter->first);       // 兩根新加入的VP所在的metal layer
			pre_vp.clear();
			pre_vp = now_vp;
			now_vp.clear();
			each_dir = (each_dir == 1) ? 0 : 1;		//下一層走向更新
		}
		now_vp = BuildVP(each_dir, pre_vp, iter->second->get_width(), 2);		//最後一根 : 倒數第二層
		which_cell->_VP.push_back(now_vp.at(0));
		which_cell->_VP_layer.push_back(iter->first);
	}
}



///
RECT design::choose_base_pin_rect(cell_type* which_cell_type, int _M2_dir)           // find out outpin  & choose outpin's straight one rect
{
	pin* out_pin;
	out_pin =which_cell_type->find_outpin();
	if (out_pin != NULL) {
		return out_pin->find_rect(_M2_dir);          // 找到直的 (if_M2_dir==1)  or 回傳_access_flag=-1的一個垃圾
	}
	else {                 //output pin =NULL  ( nearly impossible)
		RECT trect;
		trect.set_access_flag(-1);
		return trect;
	}
}

RECT design::choose_specify_dir_pin_rect(cell_type* which_cell_type, int _dir)       // find out outpin's d_dir rect
{
	pin* out_pin;
	out_pin = which_cell_type->find_outpin();
	if (out_pin != NULL) {
		for (int i = 0; i < out_pin->Get_rect_size(); ++i) {
			if (out_pin->Get_rect().at(i).horizontal_or_straight() == _dir) {
				return out_pin->Get_rect().at(i);
			}
		}
	}
	else  return RECT(0, 0, 0, 0);
	
}

bool design::check_overlap(box range, vector<value> check_array,vector<value>&_result)
{
	for (int i = 0; i < check_array.size(); ++i) {
		if (bg::intersects(range, check_array.at(i).first) == 1) {
			_result.push_back(check_array.at(i));
		}
	}
	if(_result.empty())  return false;
	else      			return true;
}


void design::offset(vector<pair<pair<double, double>, pair<double, double>>>& _now_vp, double _bias,double _low_bound, double _up_bound)
{
	//double bias = 0.05;
	int _flag_in_LB= 0;
	cout << "_bias in offset : " << _bias << endl;
	if (_now_vp.at(0).first.second > (_up_bound- _bias)) {
		_now_vp.at(0).second.second -= abs(_now_vp.at(0).first.second - (_up_bound- _bias));
		_now_vp.at(1).second.second -= abs(_now_vp.at(0).first.second - (_up_bound - _bias));
		_now_vp.at(0).first.second = _up_bound - _bias;
		_now_vp.at(1).first.second = _up_bound - _bias;
		cout << "number 1  !!!!!!!!!!!!!!!!" << endl;
	}  


	if (_now_vp.at(0).first.second < (_low_bound+ _bias)) {
		_now_vp.at(0).second.second += abs((_low_bound+ _bias) - _now_vp.at(0).first.second);
		_now_vp.at(1).second.second += abs((_low_bound + _bias) - _now_vp.at(0).first.second);
		_now_vp.at(0).first.second = _low_bound + _bias;
		_now_vp.at(1).first.second = _low_bound + _bias;
		_flag_in_LB = 1;
		cout << "number 2  !!!!!!!!!!!!!!!!" << endl;
		cout << "now_vp.at(0) num2 : " << _now_vp.at(0).first.second << "   " << _now_vp.at(0).second.second << "    " << _now_vp.at(1).first.second << "      " << _now_vp.at(1).second.second << endl;
	}
	if (_now_vp.at(0).second.second > (_up_bound- _bias)) {
		if (_flag_in_LB != 1) {
			_now_vp.at(0).first.second -= abs(_now_vp.at(0).second.second - (_up_bound - _bias));
			_now_vp.at(1).first.second -= abs(_now_vp.at(0).second.second - (_up_bound - _bias));
		}
		_now_vp.at(0).second.second = _up_bound - _bias;
		_now_vp.at(1).second.second = _up_bound - _bias;
		cout << "number 3  !!!!!!!!!!!!!!!!" << endl;
		cout << "now_vp.at(0) num3 : " << _now_vp.at(0).first.second << "   " << _now_vp.at(0).second.second << "    " << _now_vp.at(1).first.second << "      " << _now_vp.at(1).second.second << endl;
	}
	if (_now_vp.at(0).first.second <( _low_bound + _bias)) {
		_now_vp.at(0).first.second = _low_bound + _bias;
		_now_vp.at(1).first.second = _low_bound + _bias;
		cout << "number 2-2  !!!!!!!!!!!!!!!!" << endl;
		cout << "now_vp.at(0) num2-2 : " << _now_vp.at(0).first.second << "   " << _now_vp.at(0).second.second << "    " << _now_vp.at(1).first.second << "      " << _now_vp.at(1).second.second << endl;
	}


	if (_now_vp.at(0).second.second < (_low_bound+ _bias)) {
		_now_vp.at(0).first.second += abs((_low_bound + _bias) - _now_vp.at(0).second.second);
		_now_vp.at(1).first.second += abs((_low_bound + _bias) - _now_vp.at(0).second.second);
		_now_vp.at(0).second.second = _low_bound + _bias;
		_now_vp.at(1).second.second = _low_bound + _bias;
		cout << "number 4  !!!!!!!!!!!!!!!!" << endl;
	}

	cout << "now_vp.at(0) in offset : " << _now_vp.at(0).first.second << "   " << _now_vp.at(0).second.second << "    " << _now_vp.at(1).first.second << "      " << _now_vp.at(1).second.second << endl;

}
bool cmp(const pair<double, string>& p1, const pair<double, string>& p2) {
	return p1.first < p2.first;
}
void design::sort_cell_on_row()
{
	vector<pair<double, string>>  _sorted_cell_on_a_row;
	_sorted_cell_on_a_row.clear();

	for (auto iter = _all_row.begin(); iter != _all_row.end(); ++iter) {
		for (auto iter2 = iter->second->_this_row_cells.begin(); iter2 != iter->second->_this_row_cells.end(); ++iter2) {
			//Temp_Cell_Name.first = iter2->second->get_x();
			//Temp_Cell_Name.second = iter2->first;
			//_sorted_cell_on_a_row.push_back(Temp_Cell_Name);
			_sorted_cell_on_a_row.push_back(make_pair(iter2->second->get_x(), iter2->first));      //好用!
		}

		//  開始sort，
		sort(_sorted_cell_on_a_row.begin(), _sorted_cell_on_a_row.end(), cmp);
		iter->second->_begin_cell = _sorted_cell_on_a_row.at(0).second;        //寫入此row第一個cell的name
		iter->second->_end_cell = _sorted_cell_on_a_row.back().second;        //寫入此row最後一個cell的name
		//左右cell算距離，指標互指
		//先寫入左指標
		iter->second->_this_row_cells[_sorted_cell_on_a_row.at(0).second]->_left_cell.first = NULL;	//iter=this row        //_sorted_cell_on_a_row.at(0).second:  this row 上第一個cell的name
		iter->second->_this_row_cells[_sorted_cell_on_a_row.at(0).second]->_left_cell.second =abs( iter->second->get_row_site_x()- iter->second->_this_row_cells[_sorted_cell_on_a_row.at(0).second]->get_x());
		for (int i = 1; i < _sorted_cell_on_a_row.size(); ++i) {
			iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i).second]->_left_cell.first = iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i - 1).second];     //_left_cell *= 左邊那顆cell*
			iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i).second]->_left_cell.second =abs( iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i - 1).second]->get_x() + _all_cell_type_new[iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i - 1).second]->get_parent_cell_type()]->get_size() * _units		-	iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i).second]->get_x()     );    //_left_cell.second= 與左邊那顆cell最近的距離
		}

		//再寫入右邊指標
		iter->second->_this_row_cells[_sorted_cell_on_a_row.at(_sorted_cell_on_a_row.size()-1).second]->_right_cell.first = NULL;	//iter=this row        //_sorted_cell_on_a_row.at(_sorted_cell_on_a_row.size()-1).second:  this row 上最後一個cell的name
		iter->second->_this_row_cells[_sorted_cell_on_a_row.at(_sorted_cell_on_a_row.size() - 1).second]->_right_cell.second = abs(iter->second->get_row_site_x_end()		-	( iter->second->_this_row_cells[_sorted_cell_on_a_row.at(_sorted_cell_on_a_row.size() - 1).second]->get_x()	+	_all_cell_type_new[iter->second->_this_row_cells[_sorted_cell_on_a_row.at(_sorted_cell_on_a_row.size() - 1).second]->get_parent_cell_type()]->get_size() * _units)	);		//_right_cell.second= 此cell最右邊與最右邊row的底的距離
		//cout << "最後一個cell的x :  " << iter->second->_this_row_cells[_sorted_cell_on_a_row.at(_sorted_cell_on_a_row.size() - 1).second]->get_x() << endl;
		//cout << "最後一個cell的寬: " << _all_cell_type_new[iter->second->_this_row_cells[_sorted_cell_on_a_row.at(_sorted_cell_on_a_row.size() - 1).second]->get_parent_cell_type()]->get_size() * _units << endl;
		for (int i = _sorted_cell_on_a_row.size()-2;  i >=0; --i) {
			iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i).second]->_right_cell.first = iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i + 1).second];     //_right_cell *= 右邊那顆cell*
			iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i).second]->_right_cell.second = abs(iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i + 1).second]->get_x()   -  (		_all_cell_type_new[iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i).second]->get_parent_cell_type()]->get_size() * _units +  iter->second->_this_row_cells[_sorted_cell_on_a_row.at(i).second]->get_x()	  )	);    //_right_cell.second= 與右邊那顆cell最近的距離
		}

		_sorted_cell_on_a_row.clear();
	}
}
void design::output_lef_file(string outfilename)                  //沒考慮到非cell (MACRO)
{

	ofstream fout;
	fout.open(outfilename, ios::out);
	if (!fout.is_open())  cout << "output.lef is not open ! " << endl;
	else {
		for (const auto& s : _all_cell_type_new) {
			fout << "MACRO " << s.first << endl;
			fout << "    CLASS CORE ;" << endl;
			fout << "    FOREIGN " << s.first << " 0.000000 0.000000 ;" << endl;
			fout << "    ORIGIN 0.000000 0.000000 ;" << endl;
			fout << "    SIZE " << s.second->get_size() << " BY " << s.second->get_by() << " ;" << endl;
			fout << "    SYMMETRY X Y ;" << endl;
			fout << "    SITE CoreSite ; " << endl;
			for (int i = 0;i< s.second->_all_pin.size(); ++i){                           //一個pin
				fout << "    PIN " << s.second->_all_pin.at(i)->getname() << endl;
				fout << "        DIRECTION " << s.second->_all_pin.at(i)->getdir() << " ;" << endl;
				fout << "        USE " << s.second->_all_pin.at(i)->getuse()<< " ;"<< endl;
				fout << "        PORT"<< endl;

				string pre_pin_layer, now_pin_layer;
				pre_pin_layer.clear();
				now_pin_layer.clear();
				for (int j = 0; j < s.second->_all_pin.at(i)->Get_rect_size(); ++j) {          //這個pin所有的RECT
					now_pin_layer = s.second->_all_pin.at(i)->getlayer().at(j);
					if (pre_pin_layer != now_pin_layer) {
						fout << "           LAYER " << s.second->_all_pin.at(i)->getlayer().at(j) << " ;" << endl;
					}

					fout << "            RECT " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1,'x') <<
						" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1, 'y') <<
						" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'x') <<
						" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'y') << " ;" << endl;
					pre_pin_layer = now_pin_layer;
				}
				fout << "        END" << endl;
				fout<<"    END "<< s.second->_all_pin.at(i)->getname() << endl;

				string pre_layer, now_layer;
				pre_layer.clear();
				now_layer.clear();
				if (s.second->_all_pin.at(i)->_obs.size() != 0) {
					fout << "    PIN " << "t" + s.second->_all_pin.at(i)->getname() << endl;
					fout << "       DIRECTION INPUT ;" << endl;
					fout << "       USE SIGNAL ;" << endl;
					fout << "       PORT" << endl;
					for (int k = 0; k < s.second->_all_pin.at(i)->_obs.size(); ++k) {
						now_layer = s.second->_all_pin.at(i)->_obs_layer.at(k);
						if (pre_layer != now_layer) {
							fout << "        LAYER " << s.second->_all_pin.at(i)->_obs_layer.at(k) << " ;" << endl;
						}

						fout << "        RECT " << s.second->_all_pin.at(i)->_obs.at(k).Point(1,'x') << " " <<
							s.second->_all_pin.at(i)->_obs.at(k).Point(1, 'y') << " " <<
							s.second->_all_pin.at(i)->_obs.at(k).Point(2, 'x') << " " <<
							s.second->_all_pin.at(i)->_obs.at(k).Point(2, 'y') << " ;" << endl;
						pre_layer = now_layer;
					}    //所有OBS結束
					fout << "        END" << endl;
					fout<<"    END " << "t" + s.second->_all_pin.at(i)->getname() << endl;

				}       //if 此pin OBS!=0						
			}       //一個pin


			/*for (int i = 0; i < s.second->_all_OBS.size(); ++i) {                           //一個OBS
				fout << "OBS" << endl;
				string pre_obspin_layer, now_obspin_layer;
				pre_obspin_layer.clear();
				now_obspin_layer.clear();
				for (int j = 0; j < s.second->_all_OBS.at(i)->get_RECT_size(); ++j) {          //這個pin所有的RECT
					now_obspin_layer = s.second->_all_OBS.at(i)->getlayer().at(j);
					if (pre_obspin_layer != now_obspin_layer) {
						fout << "           LAYER " << s.second->_all_OBS.at(i)->getlayer().at(j) << " ;" << endl;
					}

					fout << "            RECT " << s.second->_all_OBS.at(i)->get_RECT().at(j).first.first <<
						" " << s.second->_all_OBS.at(i)->get_RECT().at(j).first.second <<
						" " << s.second->_all_OBS.at(i)->get_RECT().at(j).second.first <<
						" " << s.second->_all_OBS.at(i)->get_RECT().at(j).second.second << " ;" << endl;
					pre_obspin_layer = now_obspin_layer;
				}
				fout << "        END" << endl;

			}*/


			fout<<"END "<< s.first << endl<<endl;
		}		// _all_cell_type結束了
		fout << "END LIBRARY" << endl;
	}
}
void design::output_dodge_lef_file(string outfilename)
{
	cout << " start output_dodge_lef_file" << endl;
	ofstream fout;
	fout.open(outfilename, ios::out);
	if (!fout.is_open())  cout << "output.lef is not open ! " << endl;
	else {
		for (const auto& s : _all_cell_type_new) {
			if (s.second->_has_insert_VP == 1) {                                    // 此cell type有打VP: 此cell type的所有cell都打了不一樣的VP-->  為不同種類的cell type了
				for (int c = 0; c < s.second->_this_type_all_cell.size(); ++c) {                  // 此cell type的每個cell : 不同的新的cell type
					string _this_cell_type_name = s.second->_this_type_all_cell.at(c)->_new_cell_type_name;
					fout << "MACRO " << _this_cell_type_name << endl;
					fout << "    CLASS CORE ;" << endl;
					fout << "    FOREIGN " << _this_cell_type_name << " 0.000000 0.000000 ;" << endl;
					fout << "    ORIGIN 0.000000 0.000000 ;" << endl;
					fout << "    SIZE " << s.second->get_size() << " BY " << s.second->get_by() << " ;" << endl;
					fout << "    SYMMETRY X Y ;" << endl;
					fout << "    SITE CoreSite ; " << endl;

					for (int i = 0; i < s.second->_all_pin.size(); ++i) {                           //列出每個pin
						if (s.second->_all_pin.at(i) != s.second->find_outpin()) {             //這個pin不是打VP的output pin
							fout << "    PIN " << s.second->_all_pin.at(i)->getname() << endl;
							fout << "        DIRECTION " << s.second->_all_pin.at(i)->getdir() << " ;" << endl;
							fout << "        USE " << s.second->_all_pin.at(i)->getuse() << " ;" << endl;
							fout << "        PORT" << endl;

							string pre_pin_layer, now_pin_layer;
							pre_pin_layer.clear();
							now_pin_layer.clear();
							for (int j = 0; j < s.second->_all_pin.at(i)->Get_rect_size(); ++j) {          //這個pin所有的RECT
								now_pin_layer = s.second->_all_pin.at(i)->getlayer().at(j);
								if (pre_pin_layer != now_pin_layer) {
									fout << "           LAYER " << s.second->_all_pin.at(i)->getlayer().at(j) << " ;" << endl;
								}

								fout << "            RECT " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1, 'x') <<
									" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1, 'y') <<
									" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'x') <<
									" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'y') << " ;" << endl;
								pre_pin_layer = now_pin_layer;
							}                           //這個pin所有的RECT結束
							fout << "        END" << endl;
							fout << "    END " << s.second->_all_pin.at(i)->getname() << endl;             //  結束一個非output的 pin
						}       //  結束一個非output的 pin
						else {		        //這個pin是打了VP的output pin
							fout << "    PIN " << s.second->_all_pin.at(i)->getname() << endl;
							fout << "        DIRECTION " << s.second->_all_pin.at(i)->getdir() << " ;" << endl;
							fout << "        USE " << s.second->_all_pin.at(i)->getuse() << " ;" << endl;
							fout << "        PORT" << endl;

							fout << "           LAYER " << s.second->_this_type_all_cell.at(c)->_VP_layer.back() << " ;" << endl;
							fout << "            RECT " << s.second->_this_type_all_cell.at(c)->_VP.back().Point(1, 'x') <<
								" " << s.second->_this_type_all_cell.at(c)->_VP.back().Point(1, 'y') <<
								" " << s.second->_this_type_all_cell.at(c)->_VP.back().Point(2, 'x') <<
								" " << s.second->_this_type_all_cell.at(c)->_VP.back().Point(2, 'y') << " ;" << endl;

							fout << "        END" << endl;
							fout << "    END " << s.second->_all_pin.at(i)->getname() << endl;             //  結束一個output pin

							//這個output pin的VP要另外建成一個pin
							fout << "    PIN " << s.second->_this_type_all_cell.at(c)->_VP_pin_name << endl;
							fout << "        DIRECTION " << "INPUT" << " ;" << endl;
							fout << "        USE " << "SIGNAL" << " ;" << endl;
							fout << "        PORT" << endl;
							string pre_pin_layer, now_pin_layer;
							pre_pin_layer.clear();
							now_pin_layer.clear();
							for (int j = 0; j < s.second->_all_pin.at(i)->Get_rect_size(); ++j) {          //這個output pin原本metal 1所有的RECT
								now_pin_layer = s.second->_all_pin.at(i)->getlayer().at(j);
								if (pre_pin_layer != now_pin_layer) {
									fout << "           LAYER " << s.second->_all_pin.at(i)->getlayer().at(j) << " ;" << endl;
								}

								fout << "            RECT " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1, 'x') <<
									" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1, 'y') <<
									" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'x') <<
									" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'y') << " ;" << endl;
								pre_pin_layer = now_pin_layer;
							}            //這個output pin原本metal 1所有的RECT結束
							pre_pin_layer.clear();
							now_pin_layer.clear();
							//這個output pin上加蓋的VP  (很多個RECT, 很多層)

							for (int j = 0; j < s.second->_this_type_all_cell.at(c)->_VP.size() - 1; ++j) {          //這個cell上加蓋的VP (RECTs)
								now_pin_layer = s.second->_this_type_all_cell.at(c)->_VP_layer.at(j);
								if (pre_pin_layer != now_pin_layer) {
									fout << "           LAYER " << s.second->_this_type_all_cell.at(c)->_VP_layer.at(j) << " ;" << endl;
								}

								fout << "            RECT " << s.second->_this_type_all_cell.at(c)->_VP.at(j).Point(1, 'x') <<
									" " << s.second->_this_type_all_cell.at(c)->_VP.at(j).Point(1, 'y') <<
									" " << s.second->_this_type_all_cell.at(c)->_VP.at(j).Point(2, 'x') <<
									" " << s.second->_this_type_all_cell.at(c)->_VP.at(j).Point(2, 'y') << " ;" << endl;
								pre_pin_layer = now_pin_layer;
							}             // 這個cell上加蓋的VP (RECTs) -->變成一個新的pin  結束。

							fout << "        END" << endl;
							fout << "    END " << s.second->_this_type_all_cell.at(c)->_VP_pin_name << endl;             //  結束一個output pin的VP要另外建成一個pin
						}                     // 結束一個打了VP的output pin : 原output pin + 新的VP pin     (else)

					}
					//此cell type的一個cell的所有pin結束  (一個new cell type的所有pin結束)
					fout << "END " << _this_cell_type_name << endl << endl;
				}		//此cell type的一個cell結束  (一個new cell type結束)
			}     // 此cell type有打VP
			else {                                    //此cell type沒打VP

				fout << "MACRO " << s.first << endl;
				fout << "    CLASS CORE ;" << endl;
				fout << "    FOREIGN " << s.first << " 0.000000 0.000000 ;" << endl;
				fout << "    ORIGIN 0.000000 0.000000 ;" << endl;
				fout << "    SIZE " << s.second->get_size() << " BY " << s.second->get_by() << " ;" << endl;
				fout << "    SYMMETRY X Y ;" << endl;
				fout << "    SITE CoreSite ; " << endl;
				for (int i = 0; i < s.second->_all_pin.size(); ++i) {                           //一個pin
					fout << "    PIN " << s.second->_all_pin.at(i)->getname() << endl;
					fout << "        DIRECTION " << s.second->_all_pin.at(i)->getdir() << " ;" << endl;
					fout << "        USE " << s.second->_all_pin.at(i)->getuse() << " ;" << endl;
					fout << "        PORT" << endl;

					string pre_pin_layer, now_pin_layer;
					pre_pin_layer.clear();
					now_pin_layer.clear();
					for (int j = 0; j < s.second->_all_pin.at(i)->Get_rect_size(); ++j) {          //這個pin所有的RECT
						now_pin_layer = s.second->_all_pin.at(i)->getlayer().at(j);
						if (pre_pin_layer != now_pin_layer) {
							fout << "           LAYER " << s.second->_all_pin.at(i)->getlayer().at(j) << " ;" << endl;
						}

						fout << "            RECT " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1, 'x') <<
							" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(1, 'y') <<
							" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'x') <<
							" " << s.second->_all_pin.at(i)->Get_rect().at(j).Point(2, 'y') << " ;" << endl;
						pre_pin_layer = now_pin_layer;
					}
					fout << "        END" << endl;
					fout << "    END " << s.second->_all_pin.at(i)->getname() << endl;
				}       //一個pin
				fout << "END " << s.first << endl << endl;
			}         //此cell type沒打VP
		}                  // _all_cell_type_new 都結束了
		fout << "END LIBRARY" << endl;
		fout.close();
	}                 // 可以讀檔的else
	cout << " finish output_dodge_lef_file" << endl;
}
void design::output_dodge_def_file(string outfilename)
{    
	///- FILLER_94475 FILLCELL2 + PLACED ( 708200 1172000 ) N
	///;
	cout << " start output_dodge_def_file" << endl;
	ofstream fout;
	fout.open(outfilename, ios::out);
	if (!fout.is_open())  cout << "output.lef is not open ! " << endl;
	else {
		fout << "COMPONENTS " << _all_cell.size() << " ;" << endl;
		for (const auto &s: _all_cell) {
			fout << "- " << s.first << " ";
			if (s.second->_new_cell_type_name == "")      fout << s.second->get_parent_cell_type();
			else    fout << s.second->_new_cell_type_name;
			fout << " + "<<s.second->get_placed_or_fixed()<<" ( " << fixed<< setprecision(0)<<s.second->get_x() << " " << fixed<< setprecision(0)<< s.second->get_y()
				<< " ) " << s.second->get_orient() << endl;
			fout << ";" << endl;
		}
		fout << "END COMPONENTS" << endl;
		fout.close();
	}
	cout << " finish output_dodge_def_file" << endl;
}
void design::cout_cell_lef()
{

	cout << " _all_cell_type_new.size(): " << _all_cell_type_new.size() << endl;
	for (const auto& s : _all_cell_type_new) {
		cout << s.first << endl;
		cout << s.second->get_cell_type_name() << endl;
		cout << s.second->get_size() << "   " << s.second->get_by() << endl;
		cout << "_all_cell_type.at(i)->_all_pin.size()  : " << s.second->_all_pin.size() << endl;
		for (int j = 0; j < s.second->_all_pin.size(); ++j) {

			cout << "PIN  " << s.second->_all_pin.at(j)->getname() << "   " <<
				s.second->_all_pin.at(j)->getdir() << "   " <<
				s.second->_all_pin.at(j)->getlayer().at(0)<< "   ";                            
				for (int k = 0; k < s.second->_all_pin.at(j)->Get_rect().size(); ++k) {
					cout<< " RECT : " << s.second->_all_pin.at(j)->Get_rect().at(k).Point(1,'x')<<"  " <<
						s.second->_all_pin.at(j)->Get_rect().at(k).Point(1, 'y') << "  " <<
						s.second->_all_pin.at(j)->Get_rect().at(k).Point(2, 'x') << "  " <<
						s.second->_all_pin.at(j)->Get_rect().at(k).Point(2, 'y') << endl;
				}

		}
		cout << endl;
	}

	cout << " _all_layer.size(): " << _all_layer.size() << endl;
	for (const auto& s : _all_layer) {
		cout << s.first << "  " << s.second->get_direction() << "  " << s.second->get_spacing()<<"   "
			<< s.second->get_width() << endl;
	}

}
void design::cout_def()
{

	int _total_std_cell_num = 0;
	cout << "_diearea : " << _diearea.first.first << "  " << _diearea.first.second << " , " <<
	_diearea.second.first << "   " << _diearea.second.second << endl;
	cout << "component_num : " << _components_num << endl;
	cout << " _all_cell_type_new.size(): " << _all_cell_type_new.size() << endl;
	for (const auto& s : _all_cell_type_new) {
		cout << s.first << endl;
		cout << s.second->get_cell_type_name() << endl;
		cout << "_all_cell_type.at(i)->_this_type_all_cell.size()  : " << s.second->_this_type_all_cell.size()<< endl;
	/*	for (int j = 0; j < s.second->_this_type_all_cell.size(); ++j) {

			cout << "PIN  " << s.second->_all_pin.at(j)->getname() << "   " <<
				s.second->_all_pin.at(j)->getdir() << "   " <<
				s.second->_all_pin.at(j)->getlayer() << "   " <<
				" RECT : " << s.second->_all_pin.at(j)->get_first_RECT().first.first << "  " <<
				s.second->_all_pin.at(j)->get_first_RECT().first.second << "  " <<
				s.second->_all_pin.at(j)->get_first_RECT().second.first << "  " <<
				s.second->_all_pin.at(j)->get_first_RECT().second.second << endl;
		}*/
		cout << endl;
		_total_std_cell_num += s.second->_this_type_all_cell.size();
	}

	cout << "_total_std_cell_num: " << _total_std_cell_num << endl;


	for (const auto& s : _all_row) {
		cout << s.first <<"    "<<s.second->get_row_name()<<"    "<< s.second->get_row_site_x()<<"     "<<s.second->get_row_site_y()<<"       "
		<<"This row has how many cells ? :   " <<s.second->_this_row_cells.size()<<endl;
	}

	cout << endl;


	string temp;
	double temp_dis = 0;
	for (const auto& s : _all_row) {
		temp = s.second->_begin_cell;
		temp_dis = s.second->_this_row_cells[temp]->_right_cell.second;
		while (temp!="") {
			cout << temp << "  is separate by  "  <<temp_dis<<endl;
			if (s.second->_this_row_cells[temp]->_right_cell.first != NULL) {
				temp = s.second->_this_row_cells[temp]->_right_cell.first->get_cell_name();
				temp_dis= s.second->_this_row_cells[temp]->_right_cell.second;
			}
			else {
				temp = "";
			}
		}
		cout <<"finish 1"<< endl<< endl;
	}

	temp="";
	temp_dis = 0;

	for (const auto& s : _all_row) {
		temp = s.second->_end_cell;
		temp_dis = s.second->_this_row_cells[temp]->_left_cell.second;
		while (temp != "") {
			cout << temp << "  is separate by  " << temp_dis << endl;
			if (s.second->_this_row_cells[temp]->_left_cell.first != NULL) {
				temp = s.second->_this_row_cells[temp]->_left_cell.first->get_cell_name();
				temp_dis = s.second->_this_row_cells[temp]->_left_cell.second;
			}
			else {
				temp = "";
			}
		}
		cout << "finish 2" << endl << endl;
	}


	/*for (const auto& s : _all_row) {
		for (auto iter = s.second->_this_row_cells.begin(); iter != s.second->_this_row_cells.end(); iter++) {

			cout << iter->second->get_cell_name() << "    "<<"Right cell:  ";
				if (iter->second->_right_cell.first != NULL) {
					cout<< iter->second->_right_cell.first->get_cell_name() << endl;
				}
				else {
					cout << endl;
				}
				
		}
		cout << endl << endl << endl;
		
	}*/
	
	for (int i = 0; i < _VDD_VSS.size(); ++i) {
		cout << "NEW " << _VDD_VSS.at(i).at(1) << " " << _VDD_VSS.at(i).at(2) << " + SHAPE STRIPE ( " <<
			_VDD_VSS.at(i).at(3) << " " << _VDD_VSS.at(i).at(4) << " ) ( ";
		if (_VDD_VSS.at(i).at(5) == _VDD_VSS.at(i).at(3))    cout << "* " << _VDD_VSS.at(i).at(6) << " )" << endl;
		else  cout << _VDD_VSS.at(i).at(5) << " * )" << endl;

	}
	
}




void design::make_rtree_M1()
{
	cout << "start make_rtree_M1 " << endl;
	double _x_coordinate = 0, _y_coordinate = 0;           // in innovus: unit in lef not unit in def
	string _cellName,_pin_name;
	RECT _a_pin_rect;			// is a RECT
	vector<double>_one_pin_all_RECT_x, _one_pin_all_RECT_y;
	rTree* RTREE=new rTree;

	int _count = 0;

	for (const auto& iter : _all_cell) {
		_x_coordinate= iter.second->get_x() /_units;
		_y_coordinate = iter.second->get_y() / _units;
		_cellName = iter.first;
		for (int i = 0; i < iter.second->get_cell_type()->_all_pin.size(); ++i) {
			_one_pin_all_RECT_x.clear();
			_one_pin_all_RECT_y.clear();
			_pin_name = iter.second->get_cell_type()->_all_pin.at(i)->getname();

			for (int j = 0; j < iter.second->get_cell_type()->_all_pin.at(i)->Get_rect_size(); ++j) {
				_a_pin_rect =iter.second->get_cell_type()->_all_pin.at(i)->Get_rect().at(j);				 // is a RECT
				_one_pin_all_RECT_x.push_back(_a_pin_rect.Point(1,'x'));
				_one_pin_all_RECT_x.push_back(_a_pin_rect.Point(1,'y'));
				_one_pin_all_RECT_y.push_back(_a_pin_rect.Point(2,'x'));
				_one_pin_all_RECT_y.push_back(_a_pin_rect.Point(2,'y'));
				//search for this pin's all RECT's bounding 
			}			// one pin's all RECT have concernd

			vector<double>::iterator _max_x, _min_x, _max_y, _min_y;
			_max_x = max_element(_one_pin_all_RECT_x.begin(), _one_pin_all_RECT_x.end());
			_min_x = min_element(_one_pin_all_RECT_x.begin(), _one_pin_all_RECT_x.end());
			_max_y = max_element(_one_pin_all_RECT_y.begin(), _one_pin_all_RECT_y.end());
			_min_y = min_element(_one_pin_all_RECT_y.begin(), _one_pin_all_RECT_y.end());

			box b(point(*_min_x + _x_coordinate, *_min_y + _y_coordinate),
			point(*_max_x+ _x_coordinate, *_max_y+ _y_coordinate));

			/*cout << "box b: " << *_min_x + _x_coordinate << "   " << *_min_y + _y_coordinate << "   "
				<< *_max_x + _x_coordinate << "   " << *_max_y + _y_coordinate << endl;*/

			 //insert new value : a M1 cell's one pin in RTREE  ( chose one below )
		rTree_value.push_back(make_pair(_cellName, _pin_name));
		int _rTree_value_index = rTree_value.size() - 1;
		RTREE->insert(make_pair(b, _rTree_value_index));
			
			
		}  //finish this cell's all pins

	}   //finish all cells
	forest.insert({ "rtMetal1",RTREE });
	cout << "finish make_rtree_M1 " << endl;
}

vector<value> design::oneMetal_VP_overlap(string _rTree_index, string _CELL_NAME, string _PIN_NAME, box  _new_VP)
{
	cout << "start box with " << _rTree_index<<" overlap "<< endl;
	vector<value>_intersect_result;
	_intersect_result.clear();

		forest[_rTree_index]->query(bgi::intersects(_new_VP), back_inserter(_intersect_result));
		for (int i = 0; i < _intersect_result.size(); ++i) {
			if (rTree_value.at(_intersect_result.at(i).second).first == _CELL_NAME && rTree_value.at(_intersect_result.at(i).second).second == _PIN_NAME) {
				_intersect_result.erase(_intersect_result.begin()+i);
				break;
			}
		}
		return _intersect_result;
	}

void design::make_PG_rtree_M3M4()
{
	cout << "start make_PG_rtree_M3M4 " << endl;
	rTree* RTREE_M3= new rTree;
	rTree* RTREE_M4 = new rTree;
	double _max_x, _min_x, _max_y, _min_y;

	//  vector<vector<string>>_VDD_VSS;			//parser input  :  0: VDD(VSS),  1: Metal3,  2: 400,  3: 20000,  4: 20200,  5: 1800000,  6: 20200
	for (int i = 0; i < _VDD_VSS.size(); ++i) {
		cout << _VDD_VSS.at(i).at(3) << "  " << _VDD_VSS.at(i).at(4) << "   " << _VDD_VSS.at(i).at(5) <<"  " <<_VDD_VSS.at(i).at(6) << endl;
		_min_x = stod(_VDD_VSS.at(i).at(3)) / _units;
		_min_y = stod(_VDD_VSS.at(i).at(4)) / _units;

		if (_VDD_VSS.at(i).at(5) == _VDD_VSS.at(i).at(3)) {
			_max_x = _min_x + stod(_VDD_VSS.at(i).at(2)) / _units;
			_max_y= stod(_VDD_VSS.at(i).at(6)) / _units;
		}
		else if (_VDD_VSS.at(i).at(6) == _VDD_VSS.at(i).at(4)) {
			_max_x= stod(_VDD_VSS.at(i).at(5)) / _units;
			_max_y = _min_y + stod(_VDD_VSS.at(i).at(2)) / _units;
		}
		else { 
			cout << "nothing equal " << endl; 
		}

		//cout << _min_x << "  " << _min_y << "  " << _max_x << "  " << _max_y << endl;
		box b(point(_min_x, _min_y), point(_max_x, _max_y));
		//box b(point(_min_x, _min_y), point(_min_x + 0.5f, _min_y + 0.5f));
		
		rTree_value.push_back(make_pair(_VDD_VSS.at(i).at(0), _VDD_VSS.at(i).at(1)));
		int _rTree_value_index = rTree_value.size() - 1;
		if (_VDD_VSS.at(i).at(1) == "Metal3")		RTREE_M3->insert(make_pair(b, _rTree_value_index) );
		else if (_VDD_VSS.at(i).at(1)=="Matel4")	RTREE_M4->insert(make_pair(b, _rTree_value_index));
	}
	//  forest  push_back()  RTREE_M3, RTREE_M4  
	forest.insert({ "rtMetal3", RTREE_M3 });
	forest.insert({ "rtMetal4", RTREE_M4 });

	cout << "end make_PG_rtree_M3M4 " << endl;
	rTree* RTREE_M2 = new rTree;
	forest.insert({ "rtMetal2", RTREE_M2 });
}

void design::add_rtree(string _Metal, string _CELL_NAME, string _PIN_NAME,box _new_rect)
{
	rTree_value.push_back(make_pair(_CELL_NAME, _PIN_NAME));
	int _rTree_value_index = rTree_value.size() - 1;
	forest["rt" + _Metal]->insert(make_pair(_new_rect, _rTree_value_index));
}
