//
// Created by kamilla on 06.03.19.
//

#include"PatternCodes.h"



PatternsCodes::PatternsCodes( vector<string> a_patterns, NCS a_ncs      ){
    patterns=a_patterns;
    n_patterns = patterns.size();
    if(n_patterns > 0 )
      n_samples = patterns[0].length();
    else
      n_samples = 0;
    
    ncs = a_ncs;

    create_simplified_table();
    create_labeltype_flags();
    create_codes_table();

}

PatternsCodes::PatternsCodes() {
    patterns={};
    n_patterns = 0;
    n_samples = 0;
}


void PatternsCodes::create_labeltype_flags(){
   have_labeltype_simplified_flag.resize(n_simplified);
   have_labeltype_pattern_flag.resize(n_patterns);
   have_labeltype_simplified_flag = false;
   have_labeltype_pattern_flag = false;
   /*cout<<"@@@  create_labeltype_flags @@@";*/
   for(int p=0; p<n_patterns; p++){
       string pattern = simple_form[p];
       string lab = "";
       int s = simple_ints[p];
       /*cout<<p<<" simple_form="<<pattern<<" "<<" s="<<s<<endl;*/
       for(int l=0; l< ncs.label_types.size(); l++){
          labeltype lt = ncs.label_types[l];
          if(find(pattern.begin(), pattern.end(), lt.name)!=pattern.end()){
             have_labeltype_pattern_flag[p][l] = true;
             have_labeltype_simplified_flag[s][l] = true;
          }
       }
   }
   simple_label = {};
   for(int s=0; s<n_simplified ; s++){
       string lab = "";
       for(int l=0;l<ncs.label_types.size();l++){
          char lt = ncs.label_types[l].name;
          char dot = '.';
          lab += have_labeltype_simplified_flag[s][l]?lt:dot;
       }
       simple_label.push_back(lab);
   }
};

void PatternsCodes::print_pattern_flags(ostream& out){
   for(int p=0; p<n_patterns; p++){
      int s = simple_ints[p];
      out<<"p= "<<setw(3)<<p<<" "<<patterns[p]<<" s="<<setw(3)<<s<<" "<<simple_form[s]<<" flags= \'";
      for(int l=0;l<ncs.label_types.size();l++){
         if (have_labeltype_pattern_flag[p][l]){
        out<<ncs.label_types[l].name;
         }else{
        out<<" ";
         }
      }
      out<<"\'"<<endl;
   }
};
void PatternsCodes::print_codes(string file){
    ofstream file_with_codes (file);
    if(file_with_codes.is_open()) {
       file_with_codes<<"n_patterns=   "<<n_patterns<<endl;
       file_with_codes<<"n_simplified= "<<n_simplified<<endl;
       file_with_codes<<"n_codes=      "<<n_codes<<endl;
       for(int i=0; i<n_patterns; i++){
    for(int j=0; j<n_patterns; j++)
           file_with_codes<<setw(3)<<calc_code_fast(i,j)<<" ";
         file_with_codes<<endl;
       }
    }else{
       cerr<<"Can't open file "<<file<<" for wrinting codes table"<<endl;
    }
};


void PatternsCodes::print_simplified_patterns(ostream &out){
   out<< "    # "<<setw(n_samples)<<"GR"<<" "<<setw(ncs.label_types.size())<<"LAB"<<" "<<setw(4)<<"MULT"<<endl;
   for(int s=0; s<n_simplified ; s++){
      out<<"   "<<setw(2)<<s<<" "<<unique_simplified_patterns[s]<<" ";
      out<<simple_label[s];
      out<<" "<<setw(4)<<simple_multiplicity[s]<<endl;
   }
};


void PatternsCodes::setPatternsCodes(vector<string> a_patterns, NCS a_ncs ) {

    patterns=a_patterns;
    n_patterns = patterns.size();

    n_patterns = patterns.size();
    if(n_patterns > 0 )
      n_samples = patterns[0].length();
    else
      n_samples = 0;
    
    ncs = a_ncs;

    create_simplified_table();
    create_labeltype_flags();
    create_codes_table();
}


void PatternsCodes::create_simplified_table()
{
    simple_form={};
    unique_simplified_patterns= {};
    simple_ints = {};
    simple_multiplicity = {};
    map <string, int> simplified_map = {};
    int unique_simple_count = -1; /* will be incremented */
    int pattern_simple_int;
    for(int i =0; i<patterns.size(); i++){
       string simple_pattern = simplify_pattern(patterns[i]);
       auto seek_pattern = simplified_map.find(simple_pattern);
       if (seek_pattern == simplified_map.end() ){
          unique_simplified_patterns.push_back(simple_pattern);
          unique_simple_count++;
          pattern_simple_int = unique_simple_count;
          simplified_map[simple_pattern]=pattern_simple_int;
          simple_multiplicity.push_back(1);
       }else{
          pattern_simple_int = simplified_map[simple_pattern];
          simple_multiplicity[pattern_simple_int]++;
       }
       simple_form.push_back(simple_pattern);
       simple_ints.push_back(pattern_simple_int);
    }
    n_simplified = unique_simplified_patterns.size();
}


void PatternsCodes::create_codes_table() {
    string symbol_code;
    unsigned long code_number;

    codes.resize(n_patterns*n_patterns);
    for(int i=0; i<n_patterns; i++){
        for (int j=0; j<n_patterns; j++){
            symbol_code= ncs.calc_code(patterns[i], patterns[j]);

            if( find(codes_list.begin(), codes_list.end(), symbol_code)!=codes_list.end()   ){
                code_number=distance(codes_list.begin(), find(codes_list.begin(), codes_list.end(), symbol_code) );
            }else{
                code_number=codes_list.size();
                code_to_number[symbol_code]=code_number;
                codes_list.push_back(symbol_code);
            }
            codes[i*n_patterns+j]=code_number;
        }
    }
    n_codes = codes_list.size();
}
    

void PatternsCodes::count_different_codes(const vector <int> &  patterns, int p_test, size_t & n_diff_raw, size_t & n_diff_col){
   n_diff_col = 0;
   n_diff_raw = 0;
   Vbool codes_raw(false, n_codes);
   Vbool codes_col(false, n_codes);

   int code = calc_code_fast(p_test, p_test);
   codes_raw[code] = true; 
   codes_col[code] = true; 
   n_diff_raw++;
   n_diff_col++;

   for(int p : patterns){
      int code;
      if(p == p_test)
         continue;
      code = calc_code_fast(p, p_test);
      if(not codes_raw[code]){
         codes_raw[code] = true; 
         n_diff_raw++;
      }
      code = calc_code_fast(p_test, p);
      if(not codes_col[code]){
         codes_col[code] = true; 
         n_diff_col++;
      }
   }
};



void PatternsCodes::count_different_codes_in_vector(const vector <int> &  patterns, 
    vector <size_t> &n_diff_raw, vector <size_t> &n_diff_col){

   n_diff_raw.assign(patterns.size(), 0);
   n_diff_col.assign(patterns.size(), 0);
   int i=0;
   for(int p : patterns){
      count_different_codes(patterns, p, n_diff_raw[i], n_diff_col[i]);
      i++;
   }
};
