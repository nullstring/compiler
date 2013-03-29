#include "ll1_parser.h"

using namespace std;

/* $ symbol */
const string dollar="DOLLAR";

vector <string> splitstr(string message){
    stringstream ss(message);
    string s;
    vector <string> str;
    while(ss>>s){
        //cout<<s<<endl;
        str.push_back(s);
    }
    return str;
}

/* Calculate the follow sets of the NonTerminals */
void
Grammar::populateFollow () {
	// add dollar to the followSet of start symbol
	if (!isNonTerminal (start)) {
		cout << "Error Follow:No start non-terminal symbol in the grammar"
			 << endl;
		exit (EXIT_FAILURE);
	}

	nonTerminals[start].followSet.insert (dollar);

	map<string, NonTerminal> it;
	while (1) {
	bool change = false;
	for (it=nonTerminals.begin (); it!=nonTerminals.end (); it++) {
		vector<string> P = it->second->productions;
		for (int i=0; i<P.size (); i++) {
			stringstream ss (P[i]);
			string nt, reverse_pi;

			// P[i]->x1 x2 x3.. xn
			// 1 to n-1, although it'll go till n but the nth first will be empty
			while (ss >> nt) {
				reverse_pi += nt + " ";
				if (isNonTerminal (nt)) {
					set<string> f_next = firstOf (ss.str ());
					bool inserted = NonTerminal[nt].followSet.insert (f_next.begin (), f_next.end ()).second;
					change |= inserted;
				}
			}
		
			// TODO delete the last space	
			// n to 1
			ss.str (reverse_pi);
			while (ss >> nt) {
				if (isNonTerminal (nt) && nullable (ss.str ())) {
					set<string> followPI = it->followSet;	// follow set of the current NT
					bool inserted = NonTerminal[nt].followSet.insert (followPI.begin (), followPI.end ()).second;
					change |= inserted;
				}
			}
		}
	}

	// if no change in any follow set, break the while loop
	if (!change) break;
	}
}

/* Calculates the Nullability of a production. */
bool 
Grammar::nullable (string P) {
	if (P == "") return true;

	stringstream ss (P);
	string nt;
	ss >> nt;
	// TODO check for trailing whitespaces in the ss stream
	if (isNonTerminal (nt) && !nonTerminals[nt].nullable) return false;
	else return nullable (ss.str ());
}

/* Calculates the First set of a production.
 * Recursion shouldn't be the bottolneck
 */
set<string> &
Grammar::firstOf (string production) {
	stringstream ss (production);
	string nt;
	set<string> first;

	ss >> nt;
	if (nt != "") {
		if (isTerminal (nt)) {
			first.insert (nt);
			break;
		}

		bool isNt = isNonTerminal (nt);
		if (!isNt) {
			cout << "Error firstOf: Symbol not in the grammar"
				<< endl;
			exit (EXIT_FAILURE);
		}

		if (isNt && !nonTerminals[nt].nullable)
			return nonTerminals[nt].firstSet;

		first.insert (nonTerminals[nt].firstSet.begin (),
				nonTerminals[nt].firstSet.end ());
		set<string> f_next = firstOf (ss.str ());
		first.insert ( f_next.begin (), f_next.end ());
	}

	return first;
}

void Grammer::makeParse(){
   
    //set<string> first = firstOf();
    map<string,NonTerminal>::iterator it;
    for(it=NonTerminals.begin(); it != nonTerminals.end() ; it++){
        NonTerminal tmp = it->second;
           //single nontreminal
        for(int i=0;i<tmp.productions.size();i++){
            string prod = tmp.productions[i];
            set<string> first = firstOf(prod);
            set<string>::iterator it_set;
            bool eps_in = false;
            for(it_set = first.begin(); it_set != first.end() ; it_set++){
                string terminal = *it_set;
                
                //if epsilon
                if(s == "EPS"){
                    eps_in = true;
                }

                map[terminal] = prod
            }

            if(eps_in){ // also calculate from follow set
                set<string> follow = tmp.followSet;
                for(it_set = follow.begin(); it_set != follow.end() ; it_set++){
                    //string terminal = *it_set;   
                    map[*it_set] = prod
                }
            }
        }

    }
}

void Grammer::calcNullable(){

    bool change = false;
    while(1){
        if(change){
            change = false;
        }
        else break;

        map<string,NonTerminal>::iterator it;
        for(it=NonTerminals.begin(); it != nonTerminals.end() ; it++){
            NonTerminal tmp = it->second;
            if(tmp.nullable)
                continue;

            bool non_terminal_nullable = false;

            //single nontreminal
            for(int i=0;i<tmp.productions.size();i++){
                vector<string> v = splitstr(tmp.productions[i]);
                bool all_nullable = true;

                //single production, if any one of the production is all_nullable, the nonTerminal is nullable
                for(int j=0;j<v.size();j++){
                    if(nonTerminals.find(v[j])!=nonTerminals.end()){
                        if(!nonTerminals[v[j]].nullable){
                            all_nullable = false;
                            break;
                        }
                    } 
                    else{
                        all_nullable = false;
                        break;
                    }
                }

                if(all_nullable){
                    non_terminal_nullable = true;
                    break;
                }
            }

            if(non_terminal_nullable){
                tmp.nullable = true;
                change = true;
            }

        }
    }
}

void Grammar(char * fileName){
	
	string data;
	ifstream file(fileName,ifstream::in);
	if(!file.is_open()){
		cout << "Error Opening file";
		exit(EXIT_FAILURE);
	}

	getline(file,data);
	while(!file.eof() && data != "%%"){
		if(!file.eof() && data != "%%" && data != ""){
			if(data[0] != '%'){
				cout << "Error in Syntex of Grammar\n";
				exit(EXIT_FAILURE);
			}

			vector<string> all_token = splitstr(data);
			/*
			   %token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
			   %start translation_unit
			*/

			if(all_token[0] == "%token"){
				if(all_token.size() > 1)
				terminals.insert(all_token.begin()+1,all_token.end());
			}
			else if(all_token[0] == "%start"){
				if(all_token.size() > 1)
					start = all_token[1];
			}
//			cout << data << endl;
		}
		getline(file,data);
	}
	while(!file.eof()){
		getline(file,data);

		NonTerminal nt;

		string nonTerminalName = "";

		while(!file.eof() && data != "\t;"){

			if(!file.eof() && data != "\t;" && data != "") {

				if(data[0] != '\t'){
					nonTerminalName = data;
					
					//If non terminal in not in map
					if(isNonTerminal(nonTerminalName)){
						nt = nonTerminals[nonTerminalName];
					}
					cout <<"Token : "<< data<<endl ;
				}

				else{
					// Rules are of the form
					// \t: primary_expression
					data = data.substr(3);
					nt.production.push_back(data);
					
					if(data == "EPS")
						nt.nullable = true;

					cout <<"Rules :" << data << endl;
				}
			}
			getline(file,data);
		}

		nonTerminals[nonTerminalName] = nt;
	}



}

