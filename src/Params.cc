/*
  This file is part of the FRED system.

  Copyright (c) 2010-2015, University of Pittsburgh, John Grefenstette,
  Shawn Brown, Roni Rosenfield, Alona Fyshe, David Galloway, Nathan
  Stone, Jay DePasse, Anuroop Sriram, and Donald Burke.

  Licensed under the BSD 3-Clause license.  See the file "LICENSE" for
  more information.
*/

//
//
// File Params.cc
//

#include "Params.h"
#include "Global.h"
#include "Utils.h"
#include <math.h>
#include <string>
#include <sstream>
#include <stdio.h>

using namespace std;

char Params::param_name[MAX_PARAMS][MAX_PARAM_SIZE];
char Params::param_value[MAX_PARAMS][MAX_PARAM_SIZE];
int Params::param_count = 0;
int Params::abort_on_failure = 1;

void Params::read_psa_parameter(char* paramfile, int line_number) {
  FILE* fp;
  char name[MAX_PARAM_SIZE];

  fp = Utils::fred_open_file(paramfile);
  if(fp != NULL) {
    fprintf(stdout, "FRED reading PSA parameter file %s\n", paramfile); fflush(stdout);
    int current_line = 1;
    while (current_line < line_number) {
      int ch = 1;
      while(ch != EOF && ch != '\n') {
	ch = fgetc(fp);
      }
      if(ch == EOF) {
	Utils::fred_abort("Unexpected EOF in params file %s on line %d\n",
			  paramfile, current_line);
      }
      current_line++;
    }
    if(fscanf(fp, "%s", name) == 1) {
      if(fscanf(fp, " = %[^\n]", Params::param_value[Params::param_count]) == 1) {
	//Remove end of line comments if they are there
	string temp_str(Params::param_value[Params::param_count]);
	size_t pos;
	string whitespaces(" \t\f\v\n\r");

	pos = temp_str.find("#");
	if(pos != string::npos) {
	  temp_str = temp_str.substr(0, pos);
	}
	//trim trailing whitespace
	pos = temp_str.find_last_not_of(whitespaces);
	if(pos != string::npos) {
	  if(pos != (temp_str.length() - 1)) {
	    temp_str.erase(pos + 1);
	  }
	} else {
	  temp_str.clear(); //str is all whitespace
	}
	strcpy(Params::param_value[Params::param_count], temp_str.c_str());
          
	strcpy(Params::param_name[Params::param_count], name);
	printf("READ_PSA_PARAMETER: %s = %s\n", Params::param_name[Params::param_count],
	       Params::param_value[Params::param_count]);
	Params::param_count++;
      } else {
	Utils::fred_abort("Bad format in params file %s on line starting with %s\n",
			  paramfile, name);
      }
    } else {
      Utils::fred_abort("Bad format in params file %s on line %d\n",
			paramfile, current_line);
    }
    fclose(fp);
  } else {
    fprintf(stdout, "FRED failed reading parameter file %s\n", paramfile); fflush(stdout);
    abort();
    // Utils::fred_abort("Help!  Can't read paramfile %s\n", paramfile);
  }
}


void Params::read_parameter_file(char* paramfile) {
  FILE *fp;
  char name[MAX_PARAM_SIZE];

  fp = Utils::fred_open_file(paramfile);
  if(fp != NULL) {
    fprintf(stdout, "FRED reading parameter file %s\n", paramfile); fflush(stdout);
    while(fscanf(fp, "%s", name) == 1) {
      if(name[0] == '#') {
        int ch = 1;
        while (ch != '\n')
          ch = fgetc(fp);
        continue;
      } else if (strcmp(name, "include") == 0) {
	char includefilename[MAX_PARAM_SIZE];
	if(fscanf(fp, "%s", includefilename) == 1) {
	  read_parameter_file(includefilename);
	}
      } else {
	// printf("PARAM NAME = |%s|\n",name);fflush(stdout);
        if(fscanf(fp, " = %[^\n]", Params::param_value[Params::param_count]) == 1) {
          
          //Remove end of line comments if they are there
          string temp_str(Params::param_value[Params::param_count]);
          size_t pos;
          string whitespaces(" \t\f\v\n\r");
          
          pos = temp_str.find("#");
          if(pos != string::npos) {
            temp_str = temp_str.substr(0, pos);
          }
          
          //trim trailing whitespace
          pos = temp_str.find_last_not_of(whitespaces);
          if (pos != string::npos) {
            if(pos != (temp_str.length() - 1))
              temp_str.erase(pos + 1);
          } else {
            temp_str.clear(); //str is all whitespace
          }
          strcpy(Params::param_value[Params::param_count], temp_str.c_str());
          
          strcpy(Params::param_name[Params::param_count], name);
          if(Global::Debug > 2) {
            printf("READ_PARAMS: %s = %s\n", Params::param_name[Params::param_count],
		   Params::param_value[Params::param_count]);
          }
          Params::param_count++;
        } else {
          Utils::fred_abort("Bad format in params file %s on line starting with %s\n",
			    paramfile, name);
        }
      }
    }
  } else {
    fprintf(stdout, "FRED failed reading parameter file %s\n", paramfile); fflush(stdout);
    abort();
    // Utils::fred_abort("Help!  Can't read paramfile %s\n", paramfile);
  }
  fprintf(stdout, "FRED finished reading parameter file %s\n", paramfile); fflush(stdout);
  fclose(fp);
}


int Params::read_parameters(char* paramfile) {
  char filename[MAX_PARAM_SIZE];
  Params::param_count = 0;
  
  strcpy(filename, "$FRED_HOME/input_files/defaults");
  read_parameter_file(filename);
  read_parameter_file(paramfile);
  Params::get_param_from_string("psa_method", Global::PSA_Method);
  Params::get_param_from_string("psa_list_file", Global::PSA_List_File);
  Params::get_param_from_string("psa_sample_size", &Global::PSA_Sample_Size);
  Params::get_param_from_string("psa_sample", &Global::PSA_Sample);
  if(Global::PSA_Sample > 0) {
    FILE* fp;
    fp = Utils::fred_open_file(Global::PSA_List_File);
    if(fp != NULL) {
      char psa_param[MAX_PARAM_SIZE];
      while(fscanf(fp, "%s", psa_param) == 1) {
	sprintf(filename, "$FRED_HOME/input_files/PSA/%s/%d/%s",
		Global::PSA_Method, Global::PSA_Sample_Size, psa_param);
	read_psa_parameter(filename, Global::PSA_Sample);
      }
      fclose(fp);
    } else {
      printf("read_params: failed to open PSA list file %s\n", Global::PSA_List_File);
      abort();
    }
  }
  if(Global::Debug > 1) {
    for(int i = 0; i < Params::param_count; ++i) {
      printf("READ_PARAMS: %s = %s\n", Params::param_name[i], Params::param_value[i]);
    }
  }
  return Params::param_count;
}

int Params::get_param(char* s, int* p) {
  int found = 0;
  for(int i = 0; i < Params::param_count; ++i) {
    if(strcmp(Params::param_name[i], s) == 0) {
      if(sscanf(Params::param_value[i], "%d", p)) {
        found = 1;
      }
    }
  }
  if(found) {
    if (Global::Debug > 0) {
      printf("PARAMS: %s = %d\n", s, *p);
      fflush( stdout);
    }
    return 1;
  } else {
    if(Global::Debug > 0) {
      printf("PARAMS: %s not found\n", s);
      fflush(stdout);
    }
    if (Params::abort_on_failure) {
      Utils::fred_abort("PARAMS: %s not found\n", s); 
    }
  }
  return 0;
}

int Params::get_param(char* s, unsigned long* p) {
  int found = 0;
  for(int i = 0; i < Params::param_count; ++i) {
    if(strcmp(Params::param_name[i], s) == 0) {
      if(sscanf(Params::param_value[i], "%lu", p)) {
        found = 1;
      }
    }
  }
  if(found) {
    if(Global::Debug > 0) {
      printf("PARAMS: %s = %lu\n", s, *p);
      fflush( stdout);
    }
    return 1;
  } else {
    if(Global::Debug > 0) {
      printf("PARAMS: %s not found\n", s);
      fflush( stdout);
    }
    if (Params::abort_on_failure) {
      Utils::fred_abort("PARAMS: %s not found\n", s); 
    }
  }
  return 0;
}

int Params::get_param(char* s, double* p) {
  int found = 0;
  for(int i = 0; i < Params::param_count; ++i) {
    if(strcmp(Params::param_name[i], s) == 0) {
      if(sscanf(Params::param_value[i], "%lf", p)) {
        found = 1;
      }
    }
  }
  if(found) {
    if(Global::Debug > 0) {
      printf("PARAMS: %s = %f\n", s, *p);
      fflush( stdout);
    }
    return 1;
  } else {
    if(Global::Debug > 0) {
      printf("PARAMS: %s not found\n", s);
      fflush( stdout);
    }
    if (Params::abort_on_failure) {
      Utils::fred_abort("PARAMS: %s not found\n", s); 
    }
  }
  return 0;
}

int Params::get_param(char* s, float* p) {
  int found = 0;
  for(int i = 0; i < Params::param_count; ++i) {
    if(strcmp(Params::param_name[i], s) == 0) {
      if(sscanf(Params::param_value[i], "%f", p)) {
        found = 1;
      }
    }
  }
  if(found) {
    if(Global::Debug > 0) {
      printf("PARAMS: %s = %f\n", s, *p);
      fflush( stdout);
    }
    return 1;
  } else {
    if(Global::Debug > 0) {
      printf("PARAMS: %s not found\n", s);
      fflush( stdout);
    }
    if (Params::abort_on_failure) {
      Utils::fred_abort("PARAMS: %s not found\n", s);
    }
  }
  return 0;
}

int Params::get_param(char* s, string &p){
  int found = 0;
  for(int i = 0; i < Params::param_count; ++i) {
    if(strcmp(Params::param_name[i], s) == 0) {
      stringstream ss;
      ss << Params::param_value[i];
      if(ss.str().size() > 0){
        p = ss.str();
        found = 1;
      }
    }
  }
  if(found) {
    if(Global::Debug > 0) {
      printf("PARAMS: %s = %s\n", s, p.c_str());
      fflush( stdout);
    }
    return 1;
  } else {
    if(Global::Debug > 0) {
      printf("PARAMS: %s not found\n", s);
      fflush(stdout);
    }
    if (Params::abort_on_failure) {
      Utils::fred_abort("PARAMS: %s not found\n", s); 
    }
  }
  return 0;
}

int Params::get_param(char* s, char* p) {
  int found = 0;
  for(int i = 0; i < Params::param_count; ++i) {
    if(strcmp(Params::param_name[i], s) == 0) {
      if(strcpy(p, Params::param_value[i])) {
        found = 1;
      }
    }
  }
  if(found) {
    if(Global::Debug > 0) {
      printf("PARAMS: %s = %s\n", s, p);
      fflush( stdout);
    }
    return 1;
  } else {
    if(Global::Debug > 0) {
      printf("PARAMS: %s not found\n", s);
      fflush( stdout);
    }
    if (Params::abort_on_failure) {
      Utils::fred_abort("PARAMS: %s not found\n", s); 
    }
  }
  return 0;
}

int Params::get_param_vector(char* s, vector<int> &p){
  char str[MAX_PARAM_SIZE];
  int n;
  char* pch;
  int v;
  Params::get_param(s, str);
  pch = strtok(str," ");
  if(sscanf(pch,"%d",&n) == 1){
    for(int i = 0; i < n; ++i){
      pch = strtok(NULL," ");
      if(pch == NULL) {
        Utils::fred_abort("Help! bad param vector: %s\n", s); 
      }
      sscanf(pch,"%d",&v);
      p.push_back(v);
    }
  } else {
    Utils::fred_abort("Incorrect format for vector %s\n", s); 
  }
  return n;
}

int Params::get_param_vector(char* s, vector<double> &p){
  char str[MAX_PARAM_SIZE];
  int n = Params::get_param(s, str);
  if (n == 0) {
    return n;
  }
  char* pch = strtok(str," ");
  if (sscanf(pch, "%d", &n) == 1) {
    for (int i = 0; i < n; i++) {
      double v;
      pch = strtok (NULL, " ");
      if(pch == NULL) {
        Utils::fred_abort("Help! bad param vector: %s\n", s); 
      }
      sscanf(pch, "%lf", &v);
      p.push_back(v);
    }
  }
  else {
    Utils::fred_abort("Incorrect format for vector %s\n", s); 
  }
  return n;
}

int Params::get_param_vector(char* s, double* p) {
  char str[MAX_PARAM_SIZE];
  int n;
  char* pch;
  Params::get_param(s, str);
  pch = strtok(str, " ");
  if(sscanf(pch, "%d", &n) == 1) {
    for(int i = 0; i < n; ++i) {
      pch = strtok(NULL, " ");
      if(pch == NULL) {
        Utils::fred_abort("Help! bad param vector: %s\n", s);
      }
      sscanf(pch, "%lf", &p[i]);
    }
  } else {
    Utils::fred_abort("PARAMS: %s not found\n", s); 
  }
  return n;
}

int Params::get_param_vector(char* s, int* p) {
  char str[MAX_PARAM_SIZE];
  int n;
  char* pch;
  Params::get_param(s, str);
  pch = strtok(str, " ");
  if(sscanf(pch, "%d", &n) == 1) {
    for(int i = 0; i < n; ++i) {
      pch = strtok(NULL, " ");
      if(pch == NULL) {
        Utils::fred_abort("Help! bad param vector: %s\n", s);
      }
      sscanf(pch, "%d", &p[i]);
    }
  } else {
    Utils::fred_abort(""); 
  }
  return n;
}


int Params::get_param_vector(char* s, string* p) {
  char str[MAX_PARAM_SIZE];
  int n;
  char* pch;
  Params::get_param(s, str);
  pch = strtok(str, " ");
  if(sscanf(pch, "%d", &n) == 1) {
    for(int i = 0; i < n; ++i) {
      pch = strtok(NULL, " ");
      if(pch == NULL) {
        Utils::fred_abort("Help! bad param vector: %s\n", s);
      }
      p[i] = pch;
    }
  } else {
    Utils::fred_abort(""); 
  }
  return n;
}

//added
int Params::get_param_vector_from_string(char *s, vector < double > &p){
  //char str[MAX_PARAM_SIZE];
  int n;
  char *pch;
  double v;
  //Params::get_param(s, str);

  pch = strtok(s," ");
  if (sscanf(pch, "%d", &n) == 1) {
    for (int i = 0; i < n; i++) {
      pch = strtok (NULL, " ");
      if (pch == NULL) {
        Utils::fred_abort("Help! bad param vector: %s\n", s); 
      }
      sscanf(pch, "%lf", &v);      
      p.push_back(v);
      printf("DOUBLE::p vector element %d is %lf\n", n, v); 
    }
    for( std::vector<double>::const_iterator i = p.begin(); i != p.end(); ++i)
      printf("age!! %f \n", *i); //std::cout << *i << ' ';
    fflush(stdout);
  }
  else {
    Utils::fred_abort("Incorrect format for vector %s\n", s); 
  }
  return n;
}

int Params::get_param_vector_from_string(char *s, vector < string > &p){
  char str[MAX_PARAM_SIZE];
  int n;
  char *pch;
  string v;//string??
  char vp[MAX_PARAM_SIZE];
  
  Params::get_param(s, str);
  printf("Reading parameter vector: param %s = %s\n", s, str);
  pch = strtok(str," ");
  if (sscanf(pch, "%d", &n) == 1) {
    for (int i = 0; i < n; i++) {
      pch = strtok (NULL, " ");
      if (pch == NULL) {
        Utils::fred_abort("Help! bad param vector: %s\n", s); 
      }
      sscanf(pch, "%s", &vp);
      p.push_back(string(vp));
      printf("p vector element %d is %s\n", n, vp); 
    }
    // for( std::vector<string>::const_iterator i = p.begin(); i != p.end(); ++i)
    //   printf("age!! %f \n", *i); //std::cout << *i << ' ';
    fflush(stdout);
  }else {
    printf("N was read as %d\n", n);
    Utils::fred_abort("Incorrect format for vector %s\n", s); 
  }
  return n;
}

// end added


int Params::get_param_matrix(char* s, double*** p) {
  int n = 0;
  Params::get_param((char*)s, &n);
  if(n) {
    double* tmp;
    tmp = new double[n];
    Params::get_param_vector((char*)s, tmp);
    int temp_n = (int)sqrt((double)n);
    if(n != temp_n * temp_n) {
      Utils::fred_abort("Improper matrix dimensions: matricies must be square found dimension %i\n", n); 
    }
    n = temp_n;
    (*p) = new double*[n];
    for(int i = 0; i < n; ++i) {
      (*p)[i] = new double[n];
    }
    for(int i = 0; i < n; ++i) {
      for(int j = 0; j < n; ++j) {
        (*p)[i][j] = tmp[i * n + j];
      }
    }
    delete[] tmp;
    return n;
  }
  return -1;
}

int Params::get_param_map(char* s, map<string, double>* p) {
  int err = 0;
  char str[MAX_PARAM_SIZE];
  Params::get_param(s, str);

  stringstream pairsStream(str);
  string kv_pair;
  getline(pairsStream, kv_pair, ' '); // read number of elements

  while(getline(pairsStream, kv_pair, ' ')) {
    stringstream ss(kv_pair);
    string key, val;

    if( getline(ss, key, ':') && getline(ss, val, ':') ) {
      double valDouble = atof(val.c_str());
      //p->insert( pair<string, double> ( (char *)key.c_str(), val1) );
      (*p)[key] = valDouble;
    }
    else {
      Utils::fred_abort("Help! bad param vector: %s\n", s); 
    }
  }

  return err; // TODO: is this correct?
}

int Params::get_double_indexed_param_map(string s, int index_i, int index_j, map<string, double>* p) {
  char st[80];
  sprintf(st, "%s[%d][%d]", s.c_str(), index_i, index_j);
  int err = Params::get_param_map(st,p);
  return err;
}

bool Params::does_param_exist(char* s) {
  
  bool found = false;
  for(int i = 0; i < Params::param_count && !found; ++i) {
    if(strcmp(Params::param_name[i], s) == 0) {
      found = true;
    }
  }
  
  return found;
}

bool Params::does_param_exist(string s) {
  char st[80];
  sprintf(st,"%s",s.c_str());
  return Params::does_param_exist(st);
}
