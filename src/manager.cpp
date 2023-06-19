#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include "manager.h"
#include "parser.h"

//purely to debug stuff
template <typename T>
void print_graph(std::vector<std::pair<int,std::vector<T>>> vec){
    for(const auto& i : vec){
        std::cout<<i.first<<": ";
        for(const auto& j : i.second){
            std::cout<<j<<' ';
        }
        std::cout<<std::endl;
    }
}

/**
 * This function is responsible for initializing our graph
 * and its restrictions as requested by the user
 */
void Manager::initialize(){
    while(!execute) display_initialization();
    initialize(file_name);
};

/**
 * This function is responsible for initializing our graph
 * and its restrictions as requested by the user
 */
void Manager::initialize(std::string file_name){
    Parser p(file_name);
    grafo = p.get_grafo();
    n_arestas = p.get_edges();
    n_vertices = p.get_vertices();
};

int Manager::get_vertices(){
    return n_vertices;
}
int Manager::get_arestas(){
    return n_arestas;
}

/**
 * @brief The method that actually solves the problem
 * and shows the solution obtained
 */
void Manager::solve(){
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    welsh_powell();
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    tempo = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    display_solution();
};

int Manager::get_colors(){
    return n_colors;
}



/**
 * Prompts user for file with graph, explains format asked in file
 * and introduces problem and how its solved
 */
void Manager::display_initialization(){
    std::cout<<"Bem-vindo(a) ao nosso algoritmo de timetabling!\n"
             <<"\n1 - Casos de teste disponíveis\n"
             <<"2 - Referências dos casos de teste\n"
             <<"Por favor, digite qual dos dois gostaria de ver > ";
    char com;
    std::cin >> com;
    if(com == '1'){
        std::cout<<"\nNós temos 58 casos de teste disponíveis. Por este motivo, caso deseje mais detalhes sobre um \n"
                 <<"deles ou deseje executar algum, digite um número de 1 a 58 > ";
        int idx;
        std::cin >> idx;
        Parser p(idx);
        if(p.get_grafo().empty()) return;
        file_name = p.get_file_name();
        std::cout<<"\nNome do arquivo: "<<file_name<<std::endl;
        std::cout<<"Número de vértices: "<<p.get_vertices()<<std::endl;
        std::cout<<"Número de arestas: "<<p.get_vertices()<<std::endl;
        std::cout<<"\nDeseja executar este teste? \n(Atenção: cada linha da saída representa uma cor)\n(A saída e o tempo gasto também ficarão registrados no arquivo \"result.txt\") [Y/n] > ";
        char ex;
        std::cin>>ex;
        execute = (std::tolower(ex) != 'n');
    } else {
        std::cout   <<"REG: (De Gary Lewandowski (gary@cs.wisc.edu)) Problema baseado na alocação de registros para variáveis em códigos reais.\n"
                    <<"LEI: (De Craig Morgenstern (morgenst@riogrande.cs.tcu.edu)) Grafos de Leighton com tamanho de coloração garantido. A referência\n"
                    <<"é F.T. Leighton, Journal of Research of the National Bureau of Standards, 84: 489--505 (1979).\n\n"
                    <<"SCH: (De Gary Lewandowski (lewandow@cs.wisc.edu)) Grafos de marcar aulas, com e sem salas de estudo.\n\n"
                    <<"LAT: (De Gary Lewandowski (lewandow@cs.wisc.edu)) Problema do quadrado latino.\n\n"
                    <<"SGB: (De Michael Trick (trick@cmu.edu)) Grafos da base de grafos de Stanford de Donald Knuth (Donald \n"
                    <<"Knuth's Stanford GraphBase). Podem ser divididos em:\n\n"
                    <<"\tGrafos de livro. Dada uma obra da literatura, um grafo em que cada nó representa um personagem é criado.\n" 
                    <<"\tDois nós são conectados por uma aresta se os personagens correspondentes se encontram no livro.\n"
                    <<"\tKnuth cria os grafos para cinco obras clássicas: Anna Karenina de Tolstoy (anna), David Copperfield de Dickens\n" 
                    <<"\t(david), A Ilíada de Homero (homer), Huckleberry Finn (huck) de Twain e Os Miseráveis de Hugo (jean).\n\n"
                    <<"\tGrafos de jogos. Um grafo representando os jogos de uma temporada de futebol em uma faculdade pode ser \n"
                    <<"\tum grafo em que os nós representam cada equipe da faculdade. Dois times se conectam por uma aresta se eles\n"
                    <<"\tjogaram um contra o outro na temporada. Knuth dá o grafo para a temporada de futebol universitário de 1990.\n\n"
                    <<"\tGrafos de milhas. Estes grafo são semelhantes a grafos geométricos em que os nós são colocados no espaço e \n"
                    <<"\tdois nós se conectam se estão próximos o suficiente. Estes grafos, no entanto, não são aleatórios. Os nós são \n"
                    <<"\tum conjunto de cidades dos Estados Unidos e a distância entre eles é dada pela distância em milhas de 1947. \n"
                    <<"\tEstes grafos também existem graças a Knuth.\n\n"
                    <<"\tGrafos de dama. Dado um tabuleiro de xadrez n por n, um grafo de dama é um grafo de n^2 nós, cada um correspondendo\n"
                    <<"\ta um quadrado do tabuleiro. Dois nós se conectam por uma aresta se o quadrado correspondente se encontra na mesma \n"
                    <<"\tlinha, coluna ou diagonal. Diferente de alguns outros grafos, o problema de coloração neste grafo tem uma \n"
                    <<"\tinterpretação natural: Dado tal tabuleiro, é possível colocar n conjuntos de n damas no tabuleiro de modo que\n"
                    <<"\tnão haja duas damas do mesmo conjunto na mesma linha, coluna ou diagonal? A resposta é sim se e somente se o grafo\n"
                    <<"\ttem número de coloração n. Martin Gardner afirma sem provas que este é o caso se e somente se n não é divisível nem\n"
                    <<"\tpor 2 nem por 3. Em qualquer caso, a clique máxima no grafo não é maior que n e o valor de coloração não é menor que n.\n\n"
                    <<"MYC: (De Michael Trick (trick@cmu.edu)) Grafos baseados na transformação de Mycielski. Estes grafos são difíceis de\n" 
                    <<"resolver porque eles são \"triangle free\" (clique número 2), mas o número de coloração aumenta o tamanho do problema."<<std::endl;

        std::cout<<std::endl<<"Digite 's' para sair > ";
        char s;
        std::cin>>s;
        
    }

};

/**
 * @brief Displays the table with the solution obtained
 */
void Manager::display_solution(){
    if(!found_solution){
        std::cout<<"Ops! Não conseguimos encontrar uma solução para o seu problema :("<<std::endl;
        return;
    }
    std::ofstream out;
    out.open("result.txt");
    for(const auto& i : solucao){
        for(const auto& j : i){
            std::cout<<j+1<<' ';
            out << j+1 <<' ';
        }
        if(!i.empty()){
            std::cout<<std::endl<<std::endl;
            out << std::endl<<std::endl;
        }
    }
    std::cout<<"Tempo (em ms): "<<tempo.count()<<std::endl;
    out << "Tempo (em ms): "<<tempo.count()<<std::endl;
    std::cout<<"Número de cores usadas: "<<n_colors<<std::endl;
    out<<"Número de cores usadas: "<<n_colors<<std::endl;
};


/*
 * Checks wether or not the intersection between two 
 * sorted vectors is empty
 */
template <typename T>
bool empty_intersection(std::vector<T> a, std::vector<T> b){
    for(auto i1{0u}, i2{0u}; i1 < a.size() && i2 < b.size();){
        //if there is a common element
        if(a[i1] == b[i2]) return false;
        //advance index on the vector with smallest element
        else if(a[i1] < b[i2]) ++i1;
        else if(a[i1] > b[i2]) ++i2;
    }
    return true;
}

/**
 * @brief Algorythm used to color our graph
 * according to the restrictions given  
 */
void Manager::welsh_powell(){
    //wether or not a certain vertex was colored
    bool colored;

    //sorting vertices based on their degree (O(n log n))
    std::sort(grafo.begin(), grafo.end(), [](const std::pair<int,std::vector<int>>& a, const std::pair<int,std::vector<int>>& b) {return a.second.size() > b.second.size();});
    //if we defined a maximum
    auto n_cores{grafo.size()};
    n_colors = 1;
    //creates every color partition
    for(auto i{0u}; i<n_cores; ++i) solucao.emplace_back();
    //first vertex goes to first color
    solucao[0].push_back(grafo[0].first);
    //for each uncolored vertex (O(n))
    for(auto i{1u}; i<grafo.size(); ++i){
        //each one starts as non-colored
        colored = false;
        //for every color partition (O(n))
        for(auto& j : solucao){ 
            //if this color's adjacent vertices aren't already in color j (O(n))

            if (empty_intersection(grafo[i].second, j)) {
                colored = true;
                if(j.empty()) ++n_colors;
                j.push_back(grafo[i].first);
                std::sort(j.begin(), j.end());
                break;
            }
        }
        //if a certain vertex could not be colored, there is no solution
        if(!colored){
            found_solution = false;
            return;
        }
    }
    //if it got here, we could color all vertexes
    found_solution = true; 
};

