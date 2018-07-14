#include <bits/stdc++.h>

using namespace std;

const int dias          = 5;
const int periodos      = 6;

int profesores       ;
int cursos           ;
int salones          ;

int tam_poblacion = 300;
int iteraciones   = 200000;

double porc_cruce = 0.6;
double porc_mutac = 0.4;

const int num_inter = 5;
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
vector<pair<int,int>> padres_cruce;

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
 * ponderacion()
 * calculo de los porcentajes segun su fitness
*/
void ponderacion(){
	
	porcentajes.clear();
	
	vector<double> aux;
	double suma = 0,cal,acum = 0;
	
	for(unsigned i=0;i<poblacion.size();i++){
		cal=(1/((1+poblacion[i].fitness)));
		suma+=cal;
		aux.push_back(cal);
	}

	for(unsigned i=0;i<aux.size();i++){
		acum+=(aux[i]/suma);
		porcentajes.push_back(acum);
	}
}

/*
 * seleccion()
 * por metodo ruleta se seleccionan los N/2 mejores individuos
*/
void seleccion(){
	double alea1,alea2;
	int j,k;

	map<int,int> contador;
	siguiente_generacion.clear();
	padres_cruce.clear();

	siguiente_generacion.push_back(poblacion[poblacion.size()-1]);

	for(unsigned i=0;i<tam_poblacion/2;i++){
		alea1=rand()%100000;
		alea1/=100000;
		alea2=rand()%100000;
		alea2/=100000;
		j=0;
		k=0;
		
		while(porcentajes[j]<alea1 && j<poblacion.size())j++;
		while(porcentajes[k]<alea2 && k<poblacion.size())k++;

		padres_cruce.push_back(make_pair(j,k));

		if(contador[k]==0)siguiente_generacion.push_back(poblacion[k]);
		if(contador[j]==0)siguiente_generacion.push_back(poblacion[j]);

		contador[j]=contador[k]=1;
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

	siguiente_generacion.push_back(h1);
	siguiente_generacion.push_back(h2);

}

/*
 * cruzamiento()
 * funcion de cruce a partir de los individuos seleccionados
*/
void cruzamiento(){
	double aleat;
	//cerr<<"A"<<siguiente_generacion.size()<<endl;	

	for(int i=0;i<padres_cruce.size();i++){
		aleat = rand() % 100000;
		aleat /= 100000;
		if(aleat <= porc_cruce)
			operacionCruzamiento(poblacion[padres_cruce[i].first],poblacion[padres_cruce[i].second]);
	}

	//cerr<<"D"<<siguiente_generacion.size()<<endl;
	poblacion = siguiente_generacion;
	//poblacion.insert(poblacion.end(),siguiente_generacion.begin(),siguiente_generacion.end());
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

void elitismo(){
	sort(poblacion.begin(),poblacion.end(),ordenar);
	poblacion[0]=poblacion[poblacion.size()-1];
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
	
	//generar_mascara();

	for(int i=0;i<iteraciones;i++){
		generar_mascara();
		ponderacion();
		elitismo();
		seleccion();
		cruzamiento();
		sustitucion();
		if((i+1)%100 == 0){
			cerr<<"Generacion "<<i+1<<": "<<poblacion.size()<<" ";
			cerr<<aptitudG()<<" Mejor: "<<aptitud(poblacion[poblacion.size()-1])<<endl;
		}
		if(aptitud(poblacion[poblacion.size()-1])==0)break;
		//cerr<<endl<<string(70,'&')<<endl;
	}
	imprimir();
	cout<<aptitudG()<<endl;
	return 0;
}
