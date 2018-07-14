#include <bits/stdc++.h>

using namespace std;

const int salones       = 5;
const int dias          = 5;
const int periodos      = 6;
int profesores       ;
int cursos           ;

int tam_poblacion = 300;
int iteraciones   = 100000;

double porc_cruce = 0.6;
double porc_mutac = 0.4;

const int num_inter = 10;
/*
 * Individuo:
 * horario[salon][dia][periodo] = (profesor,UDA)
 * fitness
*/
struct individuo{
	vector<vector<vector<pair<int,int>>>> horario;
	double fitness;
};

vector<individuo> poblacion;
int limHorasP,limHorasC;

vector<double> porcentajes;
vector<individuo> siguiente_generacion;
vector<vector<vector<bool>>> mascara;

/*
 * CAPMSDP(int pos)
 * pos:el individuo en la posicion 'pos' de la poblacion
 * restriccion de profesores en 2 salones a la misma hora del mismo dia
*/
int CAPMSDP(individuo &ind){
	int cont1=0,cont2,aux;
	vector<vector<vector<pair<int,int>>>> horarioE = ind.horario;
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
 * pos:el individuo en la posicion 'pos' de la poblacion
 * restriccion de limite de horas del profesor
*/
int CALHSP(individuo &ind){
	int contR=0,contH;
	vector<vector<vector<pair<int,int>>>> horarioE = ind.horario;
	
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
 * pos:el individuo en la posicion 'pos' de la poblacion
 * restriccion de limite de horas por curso
*/
int CALHSU(individuo &ind){
	int contR=0,contH;
	vector<vector<vector<pair<int,int>>>> horarioE = ind.horario;
	
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
 * funcion del objetivo local para cada individuo
 * suma de todas las restricciones
*/
int aptitud(individuo &ind){
	return CAPMSDP(ind)+CALHSP(ind)+CALHSU(ind);
}

/*
 * aptitudG()
 * funcion del objetivo global
 * suma de las aptitudes de cada individuo
*/
int aptitudG(){
	int res=0;
	for(int i=0;i<poblacion.size();i++){
		res+=aptitud(poblacion[i]);
	}
	return res;
}

/*
 * generar_poblacion()
 * numeros aleatorios para el id del profesor y del curso para cada periodo de 
 * cada dia de cada salon de cada individuo de la poblacion
*/
void generar_poblacion( ){
	
	int posA;

	for(int i=0;i<tam_poblacion;i++){

		individuo nuevo;
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
		poblacion.push_back(nuevo);
	}
}

/*
 * imprimir()
 * imprimir la poblacion por individuo
 * cada individuo muestra el estado de su horario
*/
void imprimir(){
	for(unsigned i=0;i<poblacion.size();i++){
		cout<<"Individuo "<<i<<":\n";
		for(unsigned s=0;s<(poblacion[i].horario).size();s++){
			cout<<"Salon "<<s+1<<":\n";
			for(unsigned p=0;p<periodos;p++){
				cout<<"Periodo "<<p+1<<":\t";
				for(unsigned d=0;d<dias;d++){
					cout<<"P"<<((poblacion[i].horario)[s][d][p]).first<<"-U"<<((poblacion[i].horario)[s][d][p]).second<<"\t";
				}
				cout<<endl;
			}
		}
		cout<<string(70,'=')<<endl;
		cout<<"Fitness : "<<poblacion[i].fitness<<endl;
		cout<<string(70,'#')<<endl;
	}
}

/*
 * imprimir_procentajes()
 * funcion encargada de imprimir los porcentajes acumulados de cada individuo
*/
void imprimir_porcentajes(){
	for(unsigned i=0;i<porcentajes.size();i++){
		cout<<porcentajes[i]<<" ";
	}
}

/*
 * sumaTotal()
 * sumatoria de todos los fitness de cada individuo
*/
int sumTotal(){
	double res=0;
	for(unsigned i=0;i<poblacion.size();i++){
		res+=(1/(1+poblacion[i].fitness));
	}
	return res;
}


/*
 * elitismo()
 * calculo de los porcentajes segun su fitness
*/
void elitismo(){
	porcentajes.clear();
	double suma = sumTotal(),acum=0;
	for(unsigned i=0;i<poblacion.size();i++){
		acum+=((1/((1+poblacion[i].fitness)))/suma);
		porcentajes.push_back(acum);
	}
}

/*
 * seleccion()
 * por metodo ruleta se seleccionan los N/2 mejores individuos
*/
void seleccion(){
	double alea1;
	int j;
	siguiente_generacion.clear();
	for(unsigned i=0;i<poblacion.size();i++){
		alea1=rand()%100000;
		alea1/=100000;
		j=0;
		while(porcentajes[j]<alea1 && j<poblacion.size())j++;
		siguiente_generacion.push_back(poblacion[j]);
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
 * operacionCruzamiento(pos1,pos2)
 * hace un cruzamiento entre los individuos en las posiciones pos1 y pos2
*/
void operacionCruzamiento(individuo &p1,individuo &p2){
	
	individuo h1,h2;
	h1.horario=vector<vector<vector<pair<int,int>>>>(salones,vector<vector<pair<int,int>>>(dias,vector<pair<int,int>>(periodos,pair<int,int>())));
	h2.horario=vector<vector<vector<pair<int,int>>>>(salones,vector<vector<pair<int,int>>>(dias,vector<pair<int,int>>(periodos,pair<int,int>())));
	
	for(unsigned s=0;s<salones;s++){
		for(unsigned d=0;d<dias;d++){
			for(unsigned p=0;p<periodos;p++){
				if(mascara[s][d][p]){
					h1.horario[s][d][p] = p1.horario[s][d][p];
					h2.horario[s][d][p] = p2.horario[s][d][p];
				}
				else{
					h1.horario[s][d][p] = p2.horario[s][d][p];
					h2.horario[s][d][p] = p1.horario[s][d][p];
				}
			}
		}
	}

	int posA1,posA2,posA3;
	int posB1,posB2,posB3;

	double ale;
       	ale = rand()%100000;
	ale /= 100000;

	if(ale <= porc_mutac){
		for(int i=0;i<num_inter;i++){
			posA1=rand()%salones;
			posA2=rand()%dias;
			posA3=rand()%periodos;
			posB1=rand()%salones;
			posB2=rand()%dias;
			posB3=rand()%periodos;
			swap(h1.horario[posA1][posA2][posA3],h1.horario[posB1][posB2][posB3]);
			//swap(h1.horario[posA1][posA2],h1.horario[posB1][posB2]);
		}
	}

	ale = rand()%100000;
	ale /= 100000;

	if(ale <= porc_mutac){
		for(int i=0;i<num_inter;i++){
			posA1=rand()%salones;
			posA2=rand()%dias;
			posA3=rand()%periodos;
			posB1=rand()%salones;
			posB2=rand()%dias;
			posB3=rand()%periodos;
			swap(h2.horario[posA1][posA2][posA3],h2.horario[posB1][posB2][posB3]);
			//swap(h2.horario[posA1][posA2],h2.horario[posB1][posB2]);
		}
	}

	h1.fitness = aptitud(h1);
	h2.fitness = aptitud(h2);

	poblacion.push_back(h1);
	poblacion.push_back(h2);

}

/*
 * cruzamiento()
 * funcion de cruce a partir de los individuos seleccionados
*/
void cruzamiento(){
	double aleat;
	vector<individuo> pobl_cruz;
	for(int i=0;i<siguiente_generacion.size();i++){
		aleat = rand() % 100000;
		aleat /= 100000;
		if(aleat <= porc_cruce) pobl_cruz.push_back(siguiente_generacion[i]);
	}

	//cerr << pobl_cruz.size()<<endl;
	if(pobl_cruz.size()>=1){
		int pos1,pos2;
		for(int i=0;i<tam_poblacion/2;i++){
			pos1=rand() % pobl_cruz.size();
			pos2=rand() % pobl_cruz.size();
			operacionCruzamiento(pobl_cruz[pos1],pobl_cruz[pos2]);		
		}
	}
}

/*
 * ordenar(a,b)
 * funcion de ordenar para el sort dado dos individuos a y b
*/
bool ordenar(individuo &a,individuo &b){
	return a.fitness > b.fitness;
}

/*
 * sustitucion()
 * se sustituye los individuos con mayor aptitud
*/
void sustitucion(){
	sort(poblacion.begin(),poblacion.end(),ordenar);
	int auxT = poblacion.size()-tam_poblacion;
	poblacion.erase(poblacion.begin(),poblacion.begin()+auxT);
}

int main(){
	
	srand(time(NULL));

	profesores = 4;
	cursos     = 4;

	limHorasP = 30;
	limHorasC = 30;

	generar_poblacion();
	//generar_mascara();
	//imprimir();

	cerr<<aptitudG()<<endl;
	//cout<<string(70,'%')<<endl;
	
	for(int i=0;i<iteraciones;i++){
		generar_mascara();
		elitismo();
		seleccion();
		cruzamiento();
		sustitucion();
		if((i+1)%1000 == 0){
			cerr<<"Generacion "<<i+1<<": "<<poblacion.size()<<" ";
			cerr<<aptitudG()<<endl;
		}
		//cerr<<endl<<string(70,'&')<<endl;
	}
	imprimir();
	cout<<aptitudG()<<endl;
	return 0;
}
