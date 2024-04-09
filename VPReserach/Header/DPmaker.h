#pragma once
#include<iostream>
#include<vector>
#include<queue>
#include "cell_type.h"
using namespace std;
#define INFINITY 100000

class graph;
class node;


// 兩根為一組
class VPElement {
public:
      VPElement() { ; }
      vector<RECT> element; //可以放2 3 4 ...根
};

class edge
{
public:
      edge() {
            _to_node = NULL;
            _e_score = 0;
      };

      void set_to_node(node* t_node) { _to_node = t_node; };
      void set_e_score(double _es) { _e_score = _es; };
      double get_e_score() { return _e_score; };

private:
      double _e_score;
      node* _to_node;

};


class node
{
public:
      node() { _n_score = 0; };
      node(double _n_s) { _n_score = _n_s; };
      node(void* Obj_link) { obj_link = Obj_link; };
      node(void* Obj_link, double _n_s) { _n_score = _n_s; obj_link = Obj_link; };
      void addnewnode(node* targetnode, double _e_cost) {
            edge* tempEdge1 = new edge;
            tempEdge1->set_to_node(targetnode);
            edge* tempEdge2 = new edge;
            tempEdge2->set_to_node(this);

            tempEdge1->set_e_score(_e_cost);
            tempEdge2->set_e_score(_e_cost);
            this->_next_edges.push_back(tempEdge1);
            targetnode->_pre_edges.push_back(tempEdge2);


      };
      void set_n_score(double n_s) { _n_score = n_s; };
      double get_n_score() { return _n_score; };

private:
      double _n_score;
      vector<edge*>_pre_edges;     ////連到我的
      vector<edge*>_next_edges;     ////我往下連的
      void* obj_link;        ////本質是誰 cell or RECT?


};

class graph
{
public:
      graph() {
            node* Root = new node;
            root = Root;
      };
      //graph() {};

      void addnode2graph(node* targetnode) {
            _total_node.push_back(targetnode);
      };
      void setroot(node* r) { root = r; };
      node* getroot() { return root; };
      double calc_node_c(node* _N) {};
      double calc_edge_c(edge* _E) {};


      double pin_base_leggality(RECT _pb) {             ///////////RECT _pb or RECT *_pb

       ///infinity for totally illegal
       ///different cost for different situation pin
            if (_pb.horizontal_or_straight() == 1) {

            }
            else {

            }


      };

private:
      node* root;
      vector<node*>_total_node;
};



class VPmaker
{
public:
      VPmaker() { ; }

      VPmaker(cell* base) { base_cell = base; }

      void make() {


            // 將output pin的rect產生權重
            vector<pair<double, RECT>> pin_base = Choose_PinBase();
            // 裝著所有可能的VP組合 (使我們有機會將指標清除 記憶體釋放)
            vector<VPElement*> all_vp_choose;
            // 暫存器
            queue<node*> temp_node_queue;  //node加入graph需要
            queue<int> branch_num; //node加入graph需要。下一層，在同一個pin base底下有多少node

            // 將output pin變成node，並加入graph
            // _______L1: pin base_______  (Metal 1)
            for (int i = 0; i < pin_base.size(); ++i) {          
                  node* _t_node = new node(&pin_base.at(i).second, pin_base.at(i).first);
                  _graph.getroot()->addnewnode(_t_node, pin_base.at(i).first);
                  temp_node_queue.push(_t_node);  // 將新出生的node放入暫存器中，等等連接graph會用到
            }

            // _______L2:  VP_candidate_______  (Metal 2)
            for (int i = 0; i < pin_base.size(); ++i) {
                  // 給一個pinbase  生出一群可能的選擇，"還沒實作"
                  vector<VPElement*>one_set_candidate_VP_base = M2_VP_candidate(pin_base.at(i).second);
                  all_vp_choose.assign(one_set_candidate_VP_base.begin(), one_set_candidate_VP_base.end());
                  // 將可能的選擇變為node
                  for (int j = 0; j < one_set_candidate_VP_base.size(); j++) {
                        /*
                        * 這邊需要設計兩點
                        * 1. 新生出的node，他的權重是多少
                        * 2. 新node與上一個node相連，邊上的權重是多少
                        */
                        node* _t_node = new node(one_set_candidate_VP_base.at(i)); // node權重尚未設計
                        double edge_score = 0; // edge權重
                        temp_node_queue.front()->addnewnode(_t_node, edge_score);   // 將新node與剛剛的pinbase node連結
                        temp_node_queue.push(_t_node);                                                           //新node加入暫存器
                  }
                  branch_num.push(one_set_candidate_VP_base.size());  //紀錄這一個pinbase連接多少node
                  temp_node_queue.pop(); // 換下個pinbase
            }

            // _______L3:  VP_candidate_______  (Metal 3)
            for (int i = 0; i < pin_base.size(); i++) {
                  // 生出一群可能的選擇，"還沒實作"
                  vector<VPElement*>one_set_candidate_VP_base = M3_VP_candidate();
                  all_vp_choose.assign(one_set_candidate_VP_base.begin(), one_set_candidate_VP_base.end());
                  // 將可能的選擇變為node
                  for (int k = 0; k < branch_num.front(); k++) {
                        for (int j = 0; j < one_set_candidate_VP_base.size(); j++) {
                              /*
                              * 這邊需要設計兩點
                              * 1. 新生出的node，他的權重是多少
                              * 2. 新node與上一個node相連，邊上的權重是多少
                              */
                              node* _t_node = new node(one_set_candidate_VP_base.at(i)); // node權重尚未設計
                              double edge_score = 0; // edge權重
                              temp_node_queue.front()->addnewnode(_t_node, edge_score);   // 將新node與剛剛的pinbase node連結
                              temp_node_queue.push(_t_node);                                                           //新node加入暫存器
                        }
                        temp_node_queue.pop(); // 換下個pinbase
                  }
                  branch_num.push(one_set_candidate_VP_base.size());  //紀錄這一個pinbase連接多少node
                  branch_num.pop();
            }

            /*
            * 中間想要有幾層都行
            */

            node* end_node = new node(); //結束的node，最後的node
            while (!temp_node_queue.empty()) {
                  temp_node_queue.front()->addnewnode(end_node, 0);// edge權重設為0
                  temp_node_queue.pop();
            }
            // _______________________________________DP Graph結束_______________________________________

            /*
            * 走訪DP Graph
            */

            /*
            * 回推 找出最佳VP
            */

      }

      vector<pair<double, RECT>> Choose_PinBase() {
            vector<pair<double, RECT>> _well_pb;
            pair<double, RECT> temp_pb;

            pin* o_pin;
            o_pin = base_cell->get_cell_type()->find_outpin();
            if (o_pin != NULL) {
                  for (int i = 0; i < o_pin->Get_rect_size(); ++i) {
                        RECT i_rect = o_pin->Get_rect().at(i);
                        double pbcost = PinBaseCost(i_rect);
                        if (pbcost != INFINITY && pbcost <= 0.5) {
                              temp_pb.first = pbcost;
                              temp_pb.second = i_rect;
                              _well_pb.push_back(temp_pb);
                        }
                  }
            }
            return _well_pb;
      }

      vector<VPElement*> M3_VP_candidate() { ; }

      vector<VPElement*> M2_VP_candidate(RECT _pb) {
            int _cell_x = base_cell->get_x();                  //cell's x cooridinate
            int _cell_y = base_cell->get_y();                  //cell's y cooridinate

            if (_pb.horizontal_or_straight() == 1) {
                  box _temp_box = _pb.RECT2box(_cell_x, _cell_y);
                  vector<value> _Result2;
                  check_overlap(_temp_box, base_cell->get_buffer_overlap_m3PG(), _Result2);
            }
            else {

            }
      }

      bool check_connected() {};

      void set_BaseCell(cell* C) {
            base_cell = C;
      }
      cell* get_BaseCell() {
            return base_cell;
      }

private:
      double PinBaseCost(RECT  BasePinCandi) {                     // 沒考慮到placed的其他VP
            double _spacing = 0.5;
            if (BasePinCandi.horizontal_or_straight() == 1) {
                  if (BasePinCandi.Point(2, 'y') - BasePinCandi.Point(1, 'y') >= _spacing) {
                        if (base_cell->get_buffer_overlap_m4PG().size() != 0) {
                              box temp_box = base_cell->get_buffer_overlap_m4PG().at(0).first;
                              double score =
                                    pow(temp_box.min_corner().get < 0 >() - BasePinCandi.Point(1, 'x'), 2) +
                                    pow(temp_box.max_corner().get < 0 >() - BasePinCandi.Point(2, 'x'), 2);

                              double base_score =
                                    0.5 * pow((abs(temp_box.max_corner().get<0>() - temp_box.min_corner().get < 0 >()) - (BasePinCandi.Point(2, 'x') - BasePinCandi.Point(1, 'x'))), 2);

                              double cost = base_score / score;
                        }
                        else {             //m4PG().size() = 0
                              return 0;
                        }
                  }
                  else {
                        return INFINITY;
                  }

            }
            else {
                  if (BasePinCandi.Point(2, 'x') - BasePinCandi.Point(1, 'x') >= _spacing) {
                        double cost_m3 = -1; double cost_m4 = -1;
                        if (base_cell->get_buffer_overlap_m3PG().size() != 0) {
                              box temp_box = base_cell->get_buffer_overlap_m3PG().at(0).first;
                              double score_m3 =
                                    pow(temp_box.min_corner().get < 1 >() - BasePinCandi.Point(1, 'y'), 2) +
                                    pow(temp_box.max_corner().get < 1 >() - BasePinCandi.Point(2, 'y'), 2);

                              double base_score_m3 =
                                    0.5 * pow((abs(temp_box.max_corner().get<1>() - temp_box.min_corner().get < 1 >()) - (BasePinCandi.Point(2, 'y') - BasePinCandi.Point(1, 'y'))), 2);

                              cost_m3 = base_score_m3 / score_m3;
                        }
                        if (base_cell->get_buffer_overlap_m4PG().size() != 0) {
                              box temp_box = base_cell->get_buffer_overlap_m4PG().at(0).first;
                              double score_m4 =
                                    pow(temp_box.min_corner().get < 0 >() - BasePinCandi.Point(1, 'x'), 2) +
                                    pow(temp_box.max_corner().get < 0 >() - BasePinCandi.Point(2, 'x'), 2);

                              double base_score_m4 =
                                    0.5 * pow((abs(temp_box.max_corner().get<0>() - temp_box.min_corner().get < 0 >()) - (BasePinCandi.Point(2, 'x') - BasePinCandi.Point(1, 'x'))), 2);

                              cost_m4 = score_m4 / base_score_m4;
                        }

                        if (base_cell->get_buffer_overlap_m3PG().size() == 0 && base_cell->get_buffer_overlap_m4PG().size() == 0) {
                              return 0;
                        }
                        if (cost_m3 == -1)  return cost_m4;
                        else if (cost_m4 == -1)  return cost_m3;
                        else {
                              if (cost_m3 >= cost_m4) return cost_m3;
                              else return cost_m4;
                              //return (cost_m3 >= cost_m4) ? cost_m3 : cost_m4 ); 
                        }


                  }
                  else {
                        return INFINITY;
                  }
            }
      };
      bool check_overlap(box range, vector<value> check_array, vector<value>& _result) {
            for (int i = 0; i < check_array.size(); ++i) {
                  if (bg::intersects(range, check_array.at(i).first) == 1) {
                        _result.push_back(check_array.at(i));
                  }
            }
            if (_result.empty())  return false;
            else      			return true;
      }
      cell* base_cell;
      graph _graph;
};


