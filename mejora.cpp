#include <bits/stdc++.h>

using namespace std;

const int dias           = 5;
const int periodos       = 6;
const int limiteIntentos = 50;

int profesores       ;
int cursos           ;
int salones          ;

int tam_fuente    = 100;
int iteraciones   = 200000;

/*
 * Fuente:
 * horario[salon][dia][periodo] = (profesor,UDA)
 * fitness
*/
struct fuente{
	vector<vector<vector<pair<int,int>>>> horario;
	double fitness;
	int contIntentos;
};

vector<fuente> fuentes;
int limHorasP,limHorasC;

vector<double> porcentajes;
vector<vector<vector<bool>>> mascara;

/*
 * CAPMSDP(int pos)
 * pos: posicion de la fuente en el vector 'fuentes'
 * restriccion de profesores en 2 salones a la misma hora del mismo dia
*/
int CAPMSDP(fuente &fu){
	int cont1=0,cont2,aux;
	vector<vector<vector<pair<int,int>>>> horarioE = fu.horario;

	for(unsigned d=0;d<dias;d++){
		for(unsigned p=0;p<periodos;p++){
			for(aux=0;aux<profesores;aux++){
				cont2=0;
				for(unsigned s=0;s<salones;s++){
					if(aux == horarioE[s][d][p].first)cont2++;
				}
				if(cont2>1)cont1++;
			}
		}
	}
	return cont1;
}

/*
 * CALHSP(int pos)
 * pos:posicion de la fuente en el vector 'fuentes'
 * restriccion de limite de horas del profesor
*/
int CALHSP(fuente &fu){
	int contR=0,contH;
	vector<vector<vector<pair<int,int>>>> horarioE = fu.horario;
	
	for(int i=0;i<profesores;i++){
		contH=0;
		for(unsigned s=0;s<salones;s++){
			for(unsigned d=0;d<dias;d++){
				for(unsigned p=0;p<periodos;p++){
					if(horarioE[s][d][p].first == i)contH++;
				}
			}
		}
		if(contH != limHorasP)contR++;
	}

	return contR;
}

/*
 * CALHSU(int pos)
 * pos:posicion de la fuente en el vector 'fuentes'
 * restriccion de limite de horas por curso
*/
int CALHSU(fuente &fu){
	int contR=0,contH;
	vector<vector<vector<pair<int,int>>>> horarioE = fu.horario;
	
	for(int i=0;i<cursos;i++){
		contH=0;
		for(unsigned s=0;s<salones;s++){
			for(unsigned d=0;d<dias;d++){
				for(unsigned p=0;p<periodos;p++){
					if(horarioE[s][d][p].second == i)contH++;
				}
			}
		}
		if(contH != limHorasC)contR++;
	}

	return contR;
}

/*
 * aptitud()
 * funcion del objetivo local para cada fuente
 * suma de todas las restricciones
*/
int aptitud(fuente &fu){
	return CAPMSDP(fu)+CALHSP(fu)+CALHSU(fu);
}

/*
 * aptitudG()
 * funcion del objetivo global
 * suma total de las aptitudes de cada fuente
*/
int aptitudG(){
	int res=0;
	for(int i=0;i<fuentes.size();i++){
		res+=aptitud(fuentes[i]);
	}
	return res;
}

/*
 * generar_poblacion()
 * numeros aleatorios para el id del profesor y del curso para cada periodo de 
 * cada dia de cada salon de cada fuente
*/
void generar_poblacion( ){
	
	int posA;

	for(int i=0;i<tam_fuente;i++){

		fuente nuevo;
		nuevo.horario = vector<vector<vector<pair<int,int>>>>(salones,vector<vector<pair<int,int>>>(dias,vector<pair<int,int>>(periodos,pair<int,int>())));
		int aleatorio1,aleatorio2;
		for(unsigned s=0;s<salones;s++){
			for(unsigned d=0;d<dias;d++){
				for(unsigned p=0;p<periodos;p++){
					aleatorio1 = rand() % profesores;
					aleatorio2 = rand() % cursos;
					nuevo.horario[s][d][p] = make_pair(aleatorio1,aleatorio2);
				}
			}
		}
		nuevo.fitness = aptitud(nuevo);
		nuevo.contIntentos = 0;
		fuentes.push_back(nuevo);
	}
}

/*
 * generar_fuente()
 * funcion que retorna una nuev fuente con valores aleatorios
*/
fuente generar_fuente(){
	fuente nuevo;
	nuevo.horario = vector<vector<vector<pair<int,int>>>>(salones,vector<vector<pair<int,int>>>(dias,vector<pair<int,int>>(periodos,pair<int,int>())));
	int aleatorio1,aleatorio2;
	for(unsigned s=0;s<salones;s++){
		for(unsigned d=0;d<dias;d++){
			for(unsigned p=0;p<periodos;p++){
				aleatorio1 = rand() % profesores;
				aleatorio2 = rand() % cursos;
				nuevo.horario[s][d][p] = make_pair(aleatorio1,aleatorio2);
			}
		}
	}
	nuevo.fitness = aptitud(nuevo);
	nuevo.contIntentos = 0;
	return nuevo;
}


/*
 * imprimir()
 * imprimir la todas las fuentes
 * cada fuente muestra el estado de su solucion
*/
void imprimir(){
	for(unsigned i=0;i<fuentes.size();i++){
		cout<<"Fuente "<<i<<":\n";
		for(unsigned s=0;s<(fuentes[i].horario).size();s++){
			cout<<"Salon "<<s+1<<":\n";
			for(unsigned p=0;p<periodos;p++){
				cout<<"Periodo "<<p+1<<":\t";
				for(unsigned d=0;d<dias;d++){
					cout<<"P"<<((fuentes[i].horario)[s][d][p]).first<<"-U"<<((fuentes[i].horario)[s][d][p]).second<<"\t";
				}
				cout<<endl;
			}
		}
		cout<<string(70,'=')<<endl;
		cout<<"Fitness : "<<fuentes[i].fitness<<" Intentos: "<<fuentes[i].contIntentos<<endl;
		cout<<string(70,'#')<<endl;
	}
}

/*
 * imprimir_porcentajes()
 * funcion encargada de imprimir los porcentajes acumulados de cada fuente
*/
void imprimir_porcentajes(){
	for(unsigned i=0;i<porcentajes.size();i++){
		cerr<<porcentajes[i]<<" ";
	}
}


/*
 * ponderacion()
 * calculo de los porcentajes segun su fitness
*/
void ponderacion(){
	
	porcentajes.clear();
	
	vector<double> aux;
	double suma = 0,cal,acum = 0;
	
	for(unsigned i=0;i<fuentes.size();i++){
		cal=(1/((1+fuentes[i].fitness)));
		suma+=cal;
		aux.push_back(cal);
	}

	for(unsigned i=0;i<aux.size();i++){
		acum+=(aux[i]/suma);
		porcentajes.push_back(acum);
	}
}

/*
 * generar_mascara()
 * genera una mascara aleatoria para el cruzamiento
*/
void generar_mascara(){

	mascara = vector<vector<vector<bool>>>(salones,vector<vector<bool>>(dias,vector<bool>(periodos,0)));
	for(unsigned s=0;s<salones;s++){
		for(unsigned d=0;d<dias;d++){
			for(unsigned p=0;p<periodos;p++){
				mascara[s][d][p] = rand()%2;
			}
		}
	}
}

/*
 * operacionMezcla(pos1,pos2)
 * hace un mezcla entre dos fuentes
*/
void operacionMezcla(fuente &p1,fuente &p2){
	
	fuente h1;
	h1.horario=vector<vector<vector<pair<int,int>>>>(salones,vector<vector<pair<int,int>>>(dias,vector<pair<int,int>>(periodos,pair<int,int>())));	
	/*h1.horario = p1.horario;
	int alea;*/
	/*
	pair<int,int> v1,v2;
*/
	for(unsigned s=0;s<salones;s++){
		for(unsigned d=0;d<dias;d++){
			for(unsigned p=0;p<periodos;p++){
				
				//v1 = p1.horario[s][d][p];
				//v2 = p2.horario[s][d][p];
				
				if(mascara[s][d][p]){
					h1.horario[s][d][p] = p1.horario[s][d][p];

					//h1.horario[s][d][p] = make_pair(((v1.first+(10*profesores))+(v1.first-v2.first))%profesores,((v1.second+(10*cursos))+(v1.second-v2.second))%cursos);

					/*alea = rand()%100;
					if(alea<30){h1.horario[s][d][p].first = p2.horario[s][d][p].first;}
					else if(alea<60){h1.horario[s][d][p].second = p2.horario[s][d][p].second;}
					else {h1.horario[s][d][p] = p2.horario[s][d][p];}*/
				}
				else{
					h1.horario[s][d][p] = p2.horario[s][d][p];
					
					//h1.horario[s][d][p] = make_pair(((v1.first+(10*profesores))-(v1.first-v2.first))%profesores,((v1.second+(10*cursos))-(v1.second-v2.second))%cursos);

				}
			}
		}
	}

	h1.fitness = aptitud(h1);

	if(h1.fitness < p1.fitness){
		p1.horario = h1.horario;
		p1.fitness = h1.fitness;
		p1.contIntentos = 0;
	}
	else{
		p1.contIntentos++;
	}

}

void abejasObreras(){
	int j;
	for(unsigned i=0;i<fuentes.size();i++){
		j=rand()%fuentes.size();
		while(i==j) j=rand()%fuentes.size();
		generar_mascara();
		operacionMezcla(fuentes[i],fuentes[j]);
	}
}

void abejasObservadoras(){
	int j,k;
	double alea;
	for(unsigned i=0;i<fuentes.size();i++){
		ponderacion();
		alea = rand()%100000;
		alea /= 100000;
		j=0;
		while(porcentajes[j]<alea && j<fuentes.size()-1)j++;
		k = rand() % fuentes.size();
		while(k==j)k=rand()%fuentes.size();
		generar_mascara();
		operacionMezcla(fuentes[j],fuentes[k]);
	}
}

void abejasExploradoras(){
	for(unsigned i=0;i<fuentes.size();i++){
		if(fuentes[i].contIntentos >= limiteIntentos){
			fuentes[i] = generar_fuente();
		}
	}
}

/*
 * ordenarByFitness(a,b)
 * logica por la cual se ordenara las fuentes
*/

bool ordenarByFitness(fuente &a,fuente &b){
	return a.fitness > b.fitness;
}


/*
 * ordenar()
 * funcion encargada de ordenar las fuenntes segun su fitness
*/
void ordenar(){
	sort(fuentes.begin(),fuentes.end(),ordenarByFitness);
}

int main(){
	
	srand(time(NULL));

	int tipo;
	cin>>tipo;

	salones    = tipo;
	profesores = tipo;
	cursos     = tipo;

	limHorasP = 30;
	limHorasC = 30;

	generar_poblacion();
	//imprimir();
	cerr<<aptitudG()<<endl;

	for(int i=0;i<iteraciones;i++){

		abejasObreras();
		abejasObservadoras();
		abejasExploradoras();
		ordenar();
		if((i+1)%100 == 0){
			cerr<<"Iteracion "<<i+1<<": ";
			cerr<<aptitudG()<<" Mejor -> "<<aptitud(fuentes[fuentes.size()-1])<<endl;
		}

		if(aptitud(fuentes[fuentes.size()-1])==0)break;
		//cerr<<endl<<string(70,'&')<<endl;
	}
	imprimir();
	cout<<aptitudG()<<endl;
	return 0;
}
