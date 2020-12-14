

#ifndef BLOCKFINDER_H_INCLUDED
#define BLOCKFINDER_H_INCLUDED

//#pragma once
#include "scheme.h"
#include "tasks.h"
#include "speedo.h"
#include "schemetest.h"


#include <mutex>

class cout_locker {
  public:
     void lock()   { mtx.lock(); };
     void unlock() { mtx.unlock(); };
  private:
     static std::mutex mtx;
};
   

class BlockFinder {
public:
  vector<labeltype> types;
  int samples;
  NCS ncs;
  int min_depth;
  Scheme scheme;
  void create_tasks(  );
  vector <vector <int>> patterns;
  vector <string> patterns_text;
  int depth;
  int max_depth;
  int parallel_depth=1;
  int task_size= 200;
//  int parallel_depth=3;
//  int task_size= 20000;

  cout_locker * cout_lock;  // mutex to share cout 
  int  task_id;             // Number of task to be executed              
  string run_name;          // This name is printed to stdout 
  vector <Task4run> tasks;  // Tasks generated by create_tasks
  Task4run task;            // Task for execution by separate thread
   
  bool check_t_free;
  bool check_counter_limits;
  int  min_t_free = -1;
  bool run_task_flag;       // true after recover_from_counters           
  bool create_task_flag;    // true if tasks are created                
  bool dry_table_flag;      // Dry table in generate_initial_patterns? Defined in option --dry-table
  bool sort_patterns_flag;  // Sort patterns in generate_initial_patterns? Defined in option --sort-patterns
  string results_filename;
  ofstream *result_ofstream;
  vector <int> counter = {}; //temporary
  vector <Scheme> back_up_schemes = {};
  SchemeTest *scheme_tester = NULL;
   
  Speedo speedo_iterations;   /* Number of iterations           */
  Speedo speedo_codes;        /* Number of codes checked in try_pattern */
  Speedo speedo_results;      /* Number of results found        */
   
  map <unsigned long long, set< Scheme_compact>> result;//soe
  PatternsCodes code_table;
  int index_of_type_T;
  void setup_blockfinder();
  //void setup_code_table(CodeTable & ctable);
  vector <string> generate_all_text_patterns(int  samples, bool top = true);
  void generate_initial_patterns(vector<string> & p_text);
  void open_files();
  void start_blockfinder();
  void blockfinder_finished();
  void maincycle( Task4run & task_for_run  );
  void recover_from_counters( const Task4run &task_for_run );
  void recover_from_counters( const vector<int> &current_counters, int numbertask=-1);
  bool check_counters_reached_the_end_of_task();

  void next_iteration_output();
  bool test_scheme(const Scheme & s);
  bool test_scheme_and_next_patterns(const Scheme & scheme, const vector<int> & next_patterns);
  bool check_have_enought_t_free(const Scheme & scheme, const vector<int> & patterns_left);
  void write_result(Scheme_compact  new_scheme);
  void get_next_patterns(vector <int> & patterns, int patterns_left, int start_point, 
                            vector<int> &result);
  void get_next_patterns_speedo(vector <int> & patterns, int patterns_left, int start_point, 
                            vector<int> &result);
  void go_back();
  void go_deeper(const vector <int> & next_patterns);
  void go_parallel();
  void check_max_depth();
  void save_result();
  void find_schemes();

  BlockFinder(NCS &bncs, int bsamples); 

  BlockFinder(NCS &bncs, int bsamples, int bmin_depth, int bmin_t_free, 
                    PatternsCodes & patternscode, bool generation=true );
        //bfm? 
};



void find_schemes(int id,  BlockFinder & b, Task4run & task_for_run);

/*void find_schemes_long(int id,   int bsamples, NCS &bncs, int bmin_depth, int bmin_t_free, 
                    PatternsCodes &patternscode, vector <string> &patterns_text, 
                    vector <int> &patterns_ints, Task4run & task_for_run, cout_locker  *cl);*/


void find (const BlockFinder b);


#endif
