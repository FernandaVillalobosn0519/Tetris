#include <iostream>
#include <vector>
#include <random>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <stack>
#include <algorithm>
#include <iomanip>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"      // Linea
#define YELLOW  "\033[33m"      // Cuadrado
#define GREEN   "\033[32m"      // Z
#define BLUE    "\033[34m"      // S
#define MAGENTA "\033[35m"      // T
#define CYAN    "\033[36m"      // L
#define WHITE   "\033[37m"      // J

const string TITULO = 
	"   ___________     __         .__        \n"
	"   \\__    ___/____/  |________|__| ______\n"
	"     |    |_/ __ \\   __\\_  __ \\  |/  ___/\n"
	"     |    |\\  ___/|  |  |  | \\/  |\\___ \\ \n"
	"     |____| \\___  >__|  |__|  |__/____  >\n"
	"                \\/                    \\/ \n";

const int FILAS = 20;
const int COLUMNAS = 10;
int tablero[FILAS][COLUMNAS];

int nivel = 1;
int velocidad = 400; //velocidad en milisegundos
int puntuacion = 0;
int lineasTotalesEliminadas = 0; //variable global para contar todas las lineas eliminadas

struct Pieza{
    vector<vector<int>> forma; //representa la forma de la pieza
    int x, y;
    int tipo;
};
 
struct Jugador{ //almacenamos los puntajes y nombres de los jugadores 
    string nombre; 
    int puntuacion; 
    }; 
    
vector<Jugador> ranking; //vector para almacenar el ranking de los jugadores 

vector<string> colores = {
    RED,      // Linea
    YELLOW,   // Cuadrado
    GREEN,    // Z
    BLUE,     // S
    MAGENTA,  // T
    CYAN,     // L
    WHITE     // J
};

vector<stack<pair<int, int>>> fondo(COLUMNAS); //vector para almacenar las piezas fijas en el fondo usando pilas
Pieza pieza;

void inicializarTablero();
Pieza generarPieza(mt19937 &gen);
void dibujarTablero(const Pieza &pieza);
void moverPieza(Pieza &pieza, int dx, int dy);
bool colisiona(const Pieza &pieza, int dx, int dy);
void fijarPieza(const Pieza &pieza);
void rotarPieza(Pieza &pieza);
int eliminarLineasCompletas();
bool juegoTerminado(const Pieza &pieza);
void mostrarRanking();
void actualizarRanking(const string &nombre, int puntuacion); 

int main(){
    mt19937 gen(static_cast<unsigned int>(time(0))); //tiempo del sistema como semilla
    string nombreJugador; 
    int opcion; 
	
    while(true){ 
		do{ 
			cout << TITULO;
			cout << "\n           Menu Principal" << endl; 
			cout << "     1. Empezar Juego" << endl; 
			cout << "     2. Mostrar Ranking" << endl;
            cout << "     3. Reglas" << endl; 
			cout << "     4. Salir" << endl; 
			cout << "     Seleccione una opcion: "; 
			cin >> opcion; 
			if(cin.fail() || opcion < 1 || opcion > 3){ 
                cin.clear(); //limpiar el estado de error de cin 
			    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //ignoramos la entrada no valida 
			    system("cls");}
		}while(opcion < 1 || opcion > 4); 
        switch (opcion) { 
            case 1: 
                cout << "\n     Ingrese su nombre: "; 
                cin >> nombreJugador;
    
                puntuacion = 0; //reiniciar variables del juego
                nivel = 1;
                velocidad = 400;
                pieza = generarPieza(gen);
                inicializarTablero();
                fondo = vector<stack<pair<int, int>>>(COLUMNAS); //reiniciar el fondo
                lineasTotalesEliminadas = 0;

                while(true){
                    system("cls"); //limpia la consola
                    dibujarTablero(pieza);

                    if (_kbhit()) { //verificamos si se ha presionado una tecla
                        char tecla = _getch(); //obtenemos la tecla presionada
                        if(tecla == 27){ //tecla escape para pausa
                            cout << "Juego en pausa. Presione cualquier tecla para continuar...";
                            _getch();
                        }
                        if(tecla == 0 || tecla == -32){
                            tecla = _getch();
                            switch(tecla){
                                case 80: moverPieza(pieza, 1, 0); break; //abajo
                                case 75: moverPieza(pieza, 0, -1); break; //izquierda
                                case 77: moverPieza(pieza, 0, 1); break; //derecha
                            }
                        } else {
                            switch(tecla){
                                case 'r': rotarPieza(pieza); break; //rotar
                                case 's': moverPieza(pieza, 1, 0); break; //abajo
                                case 'a': moverPieza(pieza, 0, -1); break; //izquierda
                                case 'd': moverPieza(pieza, 0, 1); break; //derecha
                            }
                        }
                    }

                    if(colisiona(pieza, 1, 0)){
                        fijarPieza(pieza); //fijamos la pieza en el fondo
                        int lineasEliminadas = eliminarLineasCompletas();

                        lineasTotalesEliminadas += lineasEliminadas;
                        while (lineasTotalesEliminadas >= nivel * 4) {
                            nivel++;
                            velocidad = max(100, velocidad - 10);
                            cout << "Felicidades! Has alcanzado el nivel " << nivel << endl;
                            Sleep(2000);
                        }

                        if(nivel == 10){ //al alcanzar el nivel 10
                            system("cls");
                            cout << "Muy bien! Pasaste el nivel 10 y has ganado el juego." << endl;
                            actualizarRanking(nombreJugador, puntuacion);
                            mostrarRanking();
                            break;
                        }

                        pieza = generarPieza(gen);
                        if(juegoTerminado(pieza)){
                            system("cls");
                            dibujarTablero(pieza);
                            cout << "Game over :( intentalo de nuevo, "<< nombreJugador << endl;
                            actualizarRanking(nombreJugador, puntuacion); 
                            break;
                        }
                    } else{
                        moverPieza(pieza, 1, 0); //mover la pieza hacia abajo
                    }
                    Sleep(velocidad); //ajustar la velocidad del juego
                }
                break;
            case 2: 
                system("cls");
                mostrarRanking();
                system("cls");
                break; 
            case 3:
                system("cls"); 
                cout << "Reglas del Juego:\n"; 
                cout << "1. Mueve las piezas con 'a' (izquierda), 'd' (derecha), 's' (abajo) y 'r' (rotar). O tambien puedes usar las flechas\n"; 
                cout << "2. Pausa con 'Esc'.\n"; 
                cout << "3. Subes de nivel al eliminar 4 lineas x nivel.\n"; 
                cout << "4. Cada nivel aumenta la velocidad.\n"; 
                cout << "5. Ganas 100 puntos por linea eliminada.\n"; 
                cout << "6. El juego termina si las piezas alcanzan la parte superior.\n"; 
                system("pause"); //pausamos para que el usuario lea las reglas 
                system("cls");
                break;
            case 4: 
                cout << "SALIENDO..." << endl;
                return 0; 
            default: 
                cout << "Opcion no valida. Intente de nuevo." << endl;
                break; 
        }
    }
    return 0;
}


void inicializarTablero(){
    for(int i = 0; i < FILAS; i++){
        for(int j = 0; j < COLUMNAS; j++){
            tablero[i][j] = 0;
        }
    }
}

Pieza generarPieza(mt19937 &gen) {
    vector<vector<vector<int>>> formas = {
        {{1, 1, 1, 1}}, 
        {{1, 1}, {1, 1}}, 
        {{0, 1, 1}, {1, 1, 0}}, 
        {{1, 1, 0}, {0, 1, 1}}, 
        {{1, 1, 1}, {0, 1, 0}}, 
        {{1, 1, 1}, {1, 0, 0}}, 
        {{1, 1, 1}, {0, 0, 1}},
        {{1, 1, 1}, {0, 1, 1}}
    };
    uniform_int_distribution<> distribucion(0, formas.size() - 1);
    int indice = distribucion(gen);
    return {formas[indice], 0, 3, indice}; 
}

void dibujarTablero(const Pieza &pieza){
	cout << TITULO;
    cout << "          +";
    for(int j = 0; j < COLUMNAS; j++){
        cout << "--";
    }
    cout << "+" << endl;

    for(int i = 0; i < FILAS; i++){
        cout << "          |";
        for(int j = 0; j < COLUMNAS; j++){
            bool esParteDePieza = false; //parte de la pieza que cae
            for(int x = 0; x < pieza.forma.size(); x++){
                for(int y = 0; y < pieza.forma[x].size(); y++){
                    if(pieza.forma[x][y] && pieza.x + x == i && pieza.y + y == j){
                        esParteDePieza = true;
                    }
                }
            }
            
            bool esParteDelFondo = false; //verificamos si hay una pieza fija en la posicion actual
            int tipoPiezaFondo = 0;
            stack<pair<int, int>> temp = fondo[j];
            while(!temp.empty()) {
                if(temp.top().first == i){
                    esParteDelFondo = true;
                    tipoPiezaFondo = temp.top().second;
                    break;
                }
                temp.pop();
            }

            if(esParteDePieza || esParteDelFondo){
                string color = RESET; //seleccionamos el color de la pieza actual
                if(esParteDePieza){
                    color = colores[pieza.tipo]; //usamos el color de la pieza que cae
                } else if(esParteDelFondo){
                    color = colores[tipoPiezaFondo]; //usamos el color de la pieza fijada en el fondo
                }
                cout << color << "[]" << RESET;
            } else{
                cout << "  ";
            }
        }
        cout << "|" << endl;
    }
    cout << "          +";
    for (int j = 0; j < COLUMNAS; j++){
        cout << "--";
    }
    cout << "+" << endl;
    cout << "\n          |====================|\n            Puntuacion: " << puntuacion << "\n            Nivel: " << nivel << "\n          |====================|" << endl;
}

void moverPieza(Pieza &pieza, int dx, int dy){
    if (!colisiona(pieza, dx, dy)) {
        pieza.x += dx;
        pieza.y += dy;
    }
}

bool colisiona(const Pieza &pieza, int dx, int dy){
    for(int i = 0; i < pieza.forma.size(); i++){
        for(int j = 0; j < pieza.forma[i].size(); j++){
            if(pieza.forma[i][j] == 1){
                int nuevaX = pieza.x + i + dx;
                int nuevaY = pieza.y + j + dy;

                if(nuevaX < 0 || nuevaX >= FILAS || nuevaY < 0 || nuevaY >= COLUMNAS){ //verificamos los limites del tablero
                    return true;
                }

                if(nuevaY >= 0 && nuevaY < COLUMNAS && nuevaX >= 0 && nuevaX < FILAS){ //verificamos si hay una pieza fija en la nueva posicion
                    stack<pair<int, int>> temp = fondo[nuevaY];
                    while(!temp.empty()){
                        if(temp.top().first == nuevaX){
                            return true; //colision con una pieza fija
                        }
                        temp.pop();
                    }
                }
            }
        }
    }
    return false;
}

void fijarPieza(const Pieza &pieza){
    for(int i = 0; i < pieza.forma.size(); i++){
        for(int j = 0; j < pieza.forma[i].size(); j++){
            if(pieza.forma[i][j] == 1){
                fondo[pieza.y + j].push({pieza.x + i, pieza.tipo}); //guardar la posicion y el tipo de pieza
            }
        }
    }
}

void rotarPieza(Pieza &pieza){
    vector<vector<int>> nuevaForma(pieza.forma[0].size(), vector<int>(pieza.forma.size()));
    for(int i = 0; i < pieza.forma.size(); i++){
        for(int j = 0; j < pieza.forma[i].size(); j++){
            nuevaForma[j][pieza.forma.size() - 1 - i] = pieza.forma[i][j];
        }
    }
    Pieza piezaRotada = {nuevaForma, pieza.x, pieza.y};
    if (!colisiona(piezaRotada, 0, 0)){
        pieza.forma = nuevaForma;
    }
}

int eliminarLineasCompletas(){
    int lineasEliminadas = 0;
    for(int i = 0; i < FILAS; i++){
        bool lineaCompleta = true;

        for(int j = 0; j < COLUMNAS; j++){
            bool encontrada = false;
            stack<pair<int, int>> temp = fondo[j];

            while(!temp.empty()){
                if(temp.top().first == i){
                    encontrada = true;
                    break;
                }
                temp.pop();
            }

            if(!encontrada){
                lineaCompleta = false;
                break;
            }
        }

        if(lineaCompleta){
            for(int j = 0; j < COLUMNAS; j++){ //removemos la fila completa del fondo
                stack<pair<int, int>> temp;
                while(!fondo[j].empty()){
                    auto valor = fondo[j].top();
                    fondo[j].pop();
                    if(valor.first != i){
                        temp.push(valor);
                    }
                }
                
                fondo[j] = temp;
                }
            }

            puntuacion += 100;
            lineasEliminadas++;
        
            for(int j = 0; j < COLUMNAS; j++){ //movemos todas las filas superiores una posicion hacia abajo
                stack<pair<int, int>> temp, tempAux;

                while(!fondo[j].empty()){  //movemos las filas superiores hacia abajo
                    auto valor = fondo[j].top();
                    fondo[j].pop();
                    if(valor.first < i){
                        temp.push({valor.first + 1, valor.second}); // Desplazamos hacia abajo
                    } else{
                        temp.push(valor);
                    }
                }

                while (!temp.empty()) { //revertimos la pila temporal a la original
                    tempAux.push(temp.top());
                    temp.pop();
                }

                while (!tempAux.empty()) {
                    fondo[j].push(tempAux.top());
                    tempAux.pop();
                }
            }
        }
        return lineasEliminadas;
    }

bool juegoTerminado(const Pieza& pieza) {
    return colisiona(pieza, 0, 0);
}

void mostrarRanking(){
	int i = 0;
	int anchoNombre = 12;
	int anchoPuntuacion = 10;
	cout << TITULO << endl;
	cout << "\n  |=============== Ranking ===============|\n";
        for(const auto& jugador : ranking){
            i++;
            cout << "  | " << setw(2) << i << ". " 
                << left << setw(anchoNombre) << jugador.nombre 
                << ": " << right << setw(anchoPuntuacion) << jugador.puntuacion << " puntos   |\n";
        }
	cout << "  |=======================================|\n";
	cout << "\nSi deseas regresar al menu, presiona cualquier tecla";
	_getch();
}

void actualizarRanking(const string& nombre, int puntuacion) {
    bool jugadorExistente = false;

    for(auto& jugador : ranking){//verificar si el jugador ya existe en el ranking
        if(jugador.nombre == nombre){
            jugadorExistente = true;

        if(puntuacion > jugador.puntuacion){//actualizar la puntuación si la nueva es mayor
                jugador.puntuacion = puntuacion;
        }

        break;
        }
    }

    if(!jugadorExistente){ //si el jugador no existe, agregarlo al ranking
        ranking.push_back({nombre, puntuacion});
    }

    sort(ranking.begin(), ranking.end(), [](const Jugador& a, const Jugador& b){//ordenar el ranking de mayor a menor puntaje
        return a.puntuacion > b.puntuacion;
    });

    if(ranking.size() > 10){ //mantener solo los mejores 10 puntajes
        ranking.resize(10);
    }
}