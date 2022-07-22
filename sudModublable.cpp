#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

class Possibility{
	public:
		bool *possible;
		int nbp;
		
		Possibility(int new_size_2){
			bool *possible = (bool *)malloc(new_size_2*sizeof(bool));
			for(int i(0); i<new_size_2; i++){
				possible[i] = false;
			}
			this->possible = possible;
			this->nbp = new_size_2;
		}

		~Possibility(){
			free(possible);
		}
};


class Sudoku{
	public:
		int size = 3;
		int size_2 = size*size;
		int size_4 = size_2*size_2;
		Possibility **cases; 	
	
		Sudoku(int new_size){
			size = new_size;
			size_2 = size*size;
			size_4 = size_2*size_2;

			Possibility **cases = (Possibility **)malloc(size_4 * sizeof(Possibility *));

			for(int i(0); i<size_4; i++){
				cases[i] = new Possibility(size_2);
			}
			this->cases = cases;
		}

		

		~Sudoku(){
			for(int i(0); i<size_4; i++){
				delete cases[i];
			}
			free(cases);
		}
};


int value(Possibility *p){
	if(p->nbp != 1)
		return -1;
	int i(-1);
	while(!p->possible[++i]){}
	return i;
}

void affiche(Sudoku &s){
	for(int i(0); i<s.size_4; i++){
		if(s.cases[i]->nbp > 1)
			cout << "_\t";
		else
			cout << value(s.cases[i]) + 1 << "\t";
		if(i%s.size_2 == s.size_2-1)
			cout << endl;
	}
	cout << endl;
}

bool enleve(Possibility *p, int k, int possibility_size){
	if(p->possible[k]){
		p->possible[k] = false;
		p->nbp--;
	}
	for(int i(0); i<possibility_size; i++)
		if(p->possible[i]) return true;
	return false;
}

bool prt_case(Sudoku &s, int i){
	int x(i/s.size_2), y(i%s.size_2);
	queue<int> file_cases;
	int valeur = value(s.cases[i/s.size_2 * s.size_2 + y]);
	int value_before = 0;
	for(int k(0); k<s.size_2; k++){
		if(k != x){
			value_before = s.cases[k*s.size_2 + y]->nbp;
			if(!enleve(s.cases[k*s.size_2 + y], valeur, s.size_2)) return false;
			if(value_before > 1 && s.cases[k*s.size_2 + y]->nbp == 1){
				
				file_cases.push(k*s.size_2 + y);
			}
		}
		if(k != y){
			value_before = s.cases[x*s.size_2 + k]->nbp;
			if(!enleve(s.cases[x*s.size_2 + k], valeur, s.size_2)) return false;
			if(value_before > 1 && s.cases[x*s.size_2 + k]->nbp == 1){
				file_cases.push(x*s.size_2 + k);
			}
		}
		int xmod(x/s.size), ymod(y/s.size), kdiv(k/s.size), kmod(k%s.size);
		if(kdiv != x%s.size || kmod != y%s.size){
			value_before = s.cases[(xmod*s.size + kdiv) * s.size_2 + ymod * s.size + kmod]->nbp;
			if(!enleve(s.cases[(xmod*s.size + kdiv) * s.size_2 + ymod*s.size + kmod], valeur, s.size_2)) return false;
			if(value_before > 1 && s.cases[(xmod * s.size + kdiv) * s.size_2 + ymod* s.size + kmod]->nbp == 1){
				file_cases.push((xmod * s.size + kdiv) * s.size_2 + ymod * s.size + kmod);
			}
		}
	}
	while(!file_cases.empty()){
		prt_case(s, file_cases.front());
		file_cases.pop();
	}
	return true;
}

bool prt(Sudoku &s){
	for(int i(0); i<s.size_4; i++){
		if(s.cases[i]->nbp == 1 && (!prt_case(s, i))) return false;
	}
	return true;
}

bool is_done(Sudoku &s){
	for(int i(0); i<s.size_4; i++)
		if(s.cases[i]->nbp != 1)
			return false;
	return true;
}

void copie(Sudoku &s, Sudoku &ss){
	for(int i(0); i<s.size_4; i++){
		for(int j(0); j<s.size_2; j++){
			ss.cases[i]->possible[j] = s.cases[i]->possible[j];
		}
		ss.cases[i]->nbp = s.cases[i]->nbp;
	}
}

int find_attempt(Sudoku &s){
	int min(s.size_2), min_index(0);
	for(int i(0); i<s.size_4; i++){
		if(s.cases[i]->nbp != 1 && s.cases[i]->nbp < min){
			min = s.cases[i]->nbp;
			min_index = i;
		}
	}
	return min_index;
}

bool iter_tentative(Sudoku &s){
	//affiche(s);
	int i(find_attempt(s));
	//cout << "LA CASE EST : " << i << endl;
	Sudoku ss = Sudoku(s.size);
	
	queue<int> tentatives;

	for(int k(0); k<s.size_2; k++){
		if(s.cases[i]->possible[k]) tentatives.push(k);
	}

	while(!tentatives.empty()){
		int tent(tentatives.front());
		tentatives.pop();
		copie(s, ss);

		for(int k(0); k<s.size_2; k++){
			if(k != tent) ss.cases[i]->possible[k] = false;
		}
		ss.cases[i]->nbp = 1;
		
		//affiche(s);

		if(prt_case(ss, i)){
			if(is_done(ss)){
				cout << "SOLUTION : " << endl;
				affiche(ss);
				return true;
			}
			if(iter_tentative(ss)) return true;
		}
	}
	return false;
}

bool solve(Sudoku &s){
	if(!prt(s)) return false;
	if(!is_done(s)){
		if(iter_tentative(s))
			return true;
		else
			return false;
	}
	cout << "SOLUTION : " << endl;
	affiche(s);
	return true;
}

int int_parser(ifstream &file){
	int parsed = 0;
	char read('0');
	while(read != '/' && read != '&' && read != ','){ 
		if(read == '_'){
			file >> read;
			break;
		}
		parsed = 10*parsed + (read - '0');
		file >> read;
	}
	return --parsed;
}

void parse(ifstream &file){
	char read(0);
	int size(0);

	while(!file.eof()){
		while(!file.eof() && read != '#'){
			file >> read;
		}
		
		if(file.eof()) break;

		if(read == '#'){
			file >> size;
		}
		
		cout << "NEW SUDOKU\n";
		
		Sudoku s = Sudoku(size);
		
		int parsed = 0;
		for(int i(0); i<s.size_4; i++){
			parsed = int_parser(file);
			
			if(parsed != -1){
				for(int k(0); k<s.size_2; k++){
					s.cases[i]->possible[k] = (parsed == k);
				}
				s.cases[i]->nbp = 1;
			}
			else{
				for(int k(0); k<s.size_2; k++){
					s.cases[i]->possible[k] = true;
				}
				s.cases[i]->nbp = s.size_2;
			}
		}
		
		affiche(s);
		if(!solve(s))
			cout << "NO SOLUTION\n" << endl;
		

		read = 0;
	}
}

int main(){
	ifstream a("/home/charles/Documents/misc_alg/cpp_train/sudoku/test.txt");
	
	parse(a);
}
