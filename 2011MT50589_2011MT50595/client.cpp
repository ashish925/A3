#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <iostream>
#include <cstdlib>
#include <tuple>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <climits>


using namespace std;

///Handle Cases after winning
///changed serer time limit
///Handle Player 2 (Probably done)
///Decrease Walls
///Take care of time
///Use opponent's thinking time if possible
///generate children intelligently
///Handle Goal Positions
///Calculate utility of initial state if needed
///check if blocked

/* Complete the function below to print 1 integer which will be your next move 
   */
int N,M,K, time_left, player,oppNum,numLevels,radius;
bool iWon=false;////ashish change

class TwoEdges{
public:
    int p,q,r,s;
    TwoEdges(int a,int b,int c,int d){
        p = a;
        q = b;
        r = c;
        s = d;
    }
};

class Node
{
public:
    int selfPos;
    int oppPos;
    int utility;
    // int numLegal-legalMoves;
    int legalMoves[3][10][10];
    int oppLegalMoves[3][10][10];
    int wallPositions[3][11][11];
    int selfWalls;
    int oppWalls;
    int graph[82][82];
    Node(int a,int b,int e,int mov[3][10][10],int oppmov[3][10][10],int wallpos[3][11][11],int g,int h,int gr[82][82]){
        selfPos = a;
        oppPos = b;
        utility = e;
        // numLegallegalMoves = f;
        for(int i = 0;i<3;i++){
            for(int j = 0;j<10;j++){
                for(int k = 0;k<10;k++){
                    legalMoves[i][j][k] = mov[i][j][k];
                }
            }
        }
        for(int i = 0;i<3;i++){
            for(int j = 0;j<10;j++){
                for(int k = 0;k<10;k++){
                    oppLegalMoves[i][j][k] = oppmov[i][j][k];
                }
            }
        }
        for(int i = 0;i<3;i++){
            for(int j = 0;j<10;j++){
                for(int k = 0;k<10;k++){
                    wallPositions[i][j][k] = wallpos[i][j][k];
                }
            }
        }
        selfWalls = g;
        oppWalls = h;
        for(int i = 0;i<82;i++){
            for(int j = 0;j<82;j++){
                graph[i][j] = gr[i][j];
            }
        }
    }
    Node(const Node &obj){
        selfPos = obj.selfPos;
        oppPos = obj.oppPos;
        utility = obj.utility;
        // numLegallegalMoves = obj.numLegallegalMoves;
        for(int i = 0;i<3;i++){
            for(int j = 0;j<10;j++){
                for(int k = 0;k<10;k++){
                    legalMoves[i][j][k] = obj.legalMoves[i][j][k];
                }
            }
        }
        for(int i = 0;i<3;i++){
            for(int j = 0;j<10;j++){
                for(int k = 0;k<10;k++){
                    oppLegalMoves[i][j][k] = obj.oppLegalMoves[i][j][k];
                }
            }
        }
        for(int i = 0;i<3;i++){
            for(int j = 0;j<10;j++){
                for(int k = 0;k<10;k++){
                    wallPositions[i][j][k] = obj.wallPositions[i][j][k];
                }
            }
        }
        selfWalls = obj.selfWalls;
        oppWalls = obj.oppWalls;
        for(int i = 1;i<82;i++){
            for(int j = 1;j<82;j++){
                graph[i][j] = obj.graph[i][j];
            }
        }
    }
    ~Node(){
    }

    /* data */
};

int boxMapping(int a,int b){
    return 9*(a-1) + b;
}

int get_c(int a){
    int temp = a % 9;
    if(temp == 0){
        return 9;
    }
    return temp;
}

int get_r(int a){
    if(a%9==0){
        return a/9;
    }
    return a/9 + 1;
}

void printLegalMoves(Node n){
    cout << "I am at " << n.selfPos << endl;
    for(int j = 0;j<10;j++){
        for(int k = 0;k<10;k++){
            if(n.legalMoves[0][j][k] == 1){
                cout << "Move 0 " << j << " " << k << " is legal" << endl;
            }
        }
    }
}
void printLegalWalls(Node n){

    for(int j = 0;j<10;j++){
        for(int k = 0;k<10;k++){
            if(n.legalMoves[1][j][k] == 1){
                cout << "Wall 1 " << j << " " << k << " is legal" << endl;
            }
        }
    }
    for(int j = 0;j<10;j++){
        for(int k = 0;k<10;k++){
            if(n.legalMoves[2][j][k] == 1){
                cout << "Wall 2 " << j << " " << k << " is legal" << endl;
            }
        }
    }
}

int calcOppNumber(int player){
    if(player == 1){
        return 2;
    }
    else{
        return 1;
    }
}

void setNeighbours(int nhood[4],int selfPos,int wallPositions[3][11][11]){
    for(int i = 0;i<4;i++){
        nhood[i] = 0;
    }
    int r = get_r(selfPos);
    int c = get_c(selfPos);
    if(r < 9){
        if(wallPositions[1][r+1][c] == 0 && wallPositions[1][r+1][c+1] == 0){
            nhood[0] = boxMapping(r+1,c);
        }
    }
    if(r > 1){
        if(wallPositions[1][r][c] == 0 && wallPositions[1][r][c+1] == 0 ){
            nhood[1] = boxMapping(r-1,c);
        }
    }
    if(c < 9){
        if(wallPositions[2][r][c+1] == 0 && wallPositions[2][r+1][c+1] == 0){
            nhood[2] = boxMapping(r,c+1);
        }
    }
    if(c > 1){
        if(wallPositions[2][r][c] == 0 && wallPositions[2][r+1][c] == 0){
            nhood[3] = boxMapping(r,c-1);
        }
    }

}

///Update for player 2
bool isGoal(int selfPos,int playerNum){
    if(playerNum == 1){
        return (selfPos > 72);
    }
    return (selfPos < 10);
    
}

int shortest_path_length(int selfPos,int wallPositions[3][11][11],int playerNum){
    // cout << "finding path" << endl;
    int count = 0;
    vector<int> frontier;
    vector<int> newFrontier;

    int visited[82];
    for(int j = 0; j<82; j++){
        visited[j] = 0;
    }
    

    int nhood[4];
    for(int i = 0;i<4;i++){
        nhood[i] = 0;
    }
    if(isGoal(selfPos,playerNum)){
        return count;
    }
    frontier.push_back(selfPos);
    while(true){
        count += 1;
        if(frontier.size()==0){
            return INT_MAX;
        }
         // cout << "Count = " << count << " size "<< frontier.size() <<endl;
        for(int k = 0;k<frontier.size();k++){
            setNeighbours(nhood,frontier[k],wallPositions);
            for(int i = 0;i < 4;i++){
                if(nhood[i] > 0){
                    if(visited[nhood[i]] == 0){
                        if(isGoal(nhood[i],playerNum)){
                            return count;
                        }
                        else{
                            newFrontier.push_back(nhood[i]);
                        }
                    }
                }
            }
            visited[frontier[k]] = 1;
        }
        frontier.clear();
        frontier.swap(newFrontier);
        newFrontier.clear();

    }
    cout << "Found path" << endl;
    return 0;
}



TwoEdges getTwoEdges(int m,int r,int c){
    if(m == 1){
        return TwoEdges(boxMapping(r-1,c-1),boxMapping(r,c-1),boxMapping(r-1,c),boxMapping(r,c));
    }
    return TwoEdges(boxMapping(r-1,c-1),boxMapping(r-1,c),boxMapping(r,c-1),boxMapping(r,c));
}



void setLegalMoves(int legalMoves[3][10][10],int wallPositions[3][11][11],int selfPos,int oppPos,int playerNum,int selfWalls,int oppWalls){

    for(int i = 0;i<3;i++){
        for(int j = 0;j<10;j++){
            for(int k = 0;k<10;k++){
                legalMoves[i][j][k] = 0;
            }
        }
    }
    int r=get_r(selfPos);
    int c=get_c(selfPos);
    int orow=get_r(oppPos);
    int ocol=get_c(oppPos);

    // cout << "SLG called from self = " << selfPos << " and opp = " << oppPos << endl;
    int availableWalls;
    if(player == playerNum){
        availableWalls = selfWalls;
    }
    else{
        availableWalls = oppWalls;
    }

    if(availableWalls > 0){
        for (int i=2;i<=9;i++)
        { 
            for(int j=2;j<=9;j++){
            ///change 9 accrdingly
                //int rad = (orow-i)*(orow-i) + (ocol-j)*(ocol-j);
                int manDist=abs(orow-i)+abs(ocol-j);
                if(manDist < radius){ ////ashish change
                    if(wallPositions[1][i][j] != 1 && wallPositions[1][i][j-1]!=1 && wallPositions[1][i][j+1]!=1 && wallPositions[2][i][j]!=1){
                    // cout << "checking wall " << 1 << " " << i << " " << j << " radius = " << radius << " orow = " << orow << " ocol = " << ocol << endl;
                        wallPositions[1][i][j] = 1;
                        if(player == 1){
                        // cout << "i : " <<i <<" j :"<<j<< endl;
                        ///change 82 accordingy
                        // cout << "finding path" << endl;
                            if((shortest_path_length(selfPos,wallPositions,1) <= 82) && (shortest_path_length(oppPos,wallPositions,2) <= 82)){
                                legalMoves[1][i][j]=1;

                            }
                        }
                        else{
                            if((shortest_path_length(selfPos,wallPositions,2) <= 82) && (shortest_path_length(oppPos,wallPositions,1) <= 82)){
                                legalMoves[1][i][j]=1;

                            }
                        }
                        wallPositions[1][i][j] = 0;
                    // cout << "checked" << endl;
                    }

                    if(wallPositions[2][i][j] != 1 && wallPositions[2][i-1][j]!=1 && wallPositions[2][i+1][j]!=1 && wallPositions[1][i][j]!=1){
                        wallPositions[2][i][j] = 1;
                        if(player == 1){
                            if((shortest_path_length(selfPos,wallPositions,1) <= 82) && (shortest_path_length(oppPos,wallPositions,2) <= 82)){
                                legalMoves[2][i][j]=1;

                            }
                        }
                        else{
                            if((shortest_path_length(selfPos,wallPositions,2) <= 82) && (shortest_path_length(oppPos,wallPositions,1) <= 82)){
                                legalMoves[2][i][j]=1;

                            }
                        }
                        wallPositions[2][i][j] = 0;
                    }

                }
            }
        }
    }
    // cout << "legal walls updated " << endl;
    if(legalMoves[0][0][6] == 1){
        cout << "4 Invalid move legalized" << endl;
    }
    if(r<9)
    {
        if (wallPositions[1][r+1][c] != 1 && wallPositions[1][r+1][c+1] != 1 )
        {
            if(!(r+1 == orow && c == ocol)){
                legalMoves[0][r+1][c]=1;
                // cout << "Case1: Move 0 " << r+1 << " " << c << " is legal" << endl;
            }
            else {
                if(wallPositions[1][r+2][c] != 1 && wallPositions[1][r+2][c+1] != 1 )
                {   
                    // cout << "r = " << r << " c = " << c << endl;
                    // cout << " Wall 1 = " << wallPositions[1][r+2][c]  << " Wall 2 = " << wallPositions[1][r+2][c+1] << endl;
                    if(r+1<9){
                        legalMoves[0][r+2][c]=1;
                        // cout << "Case1: Move 0 " << r+2 << " " << c << " is legal" << endl;
                    }
                    else{
                        if(wallPositions[2][r+1][c+1]!=1 && wallPositions[2][r+2][c+1]!=1 && c <9){
                            legalMoves[0][r+1][c+1]=1;
                            // cout << "Case1: Move 0 " << r+1 << " " << c+1 << " is legal" << endl;
                        }
                        if(wallPositions[2][r+1][c]!=1 && wallPositions[2][r+2][c]!=1 && c >1){
                            legalMoves[0][r+1][c-1]=1;
                            // cout << "Case1: Move 0 " << r+1 << " " << c << " is legal" << endl;
                        }
                    }
                }
                else{

                    if(wallPositions[2][r+1][c+1]!=1 && wallPositions[2][r+2][c+1]!=1 && c <9){
                        legalMoves[0][r+1][c+1]=1;
                        // cout << "Case1: Move 0 " << r+1 << " " << c+1 << " is legal" << endl;
                    }
                    if(wallPositions[2][r+1][c]!=1 && wallPositions[2][r+2][c]!=1 && c >1){
                        legalMoves[0][r+1][c-1]=1;
                        // cout << "Case1: Move 0 " << r+1 << " " << c << " is legal" << endl;
                    }
                }
            }
        }
    }
    //cout << "r > 9" << endl;
    if(legalMoves[0][0][6] == 1){
        cout << "3 Invalid move legalized" << endl;
    }
    if(r>1)
    {
        if (wallPositions[1][r][c] != 1 && wallPositions[1][r][c+1] != 1 )
        {
            if(!(r-1 == orow && c == ocol)){
                legalMoves[0][r-1][c]=1;
                // cout << "Case2: Move 0 " << r-1 << " " << c << " is legal" << endl;

            }
            else {
                if(wallPositions[1][r-1][c] != 1 && wallPositions[1][r-1][c+1] != 1 ){
                    if(r>2){
                        legalMoves[0][r-2][c]=1;
                        // cout << "Case2: Move 0 " << r-2 << " " << c << " is legal" << endl;
                    }
                    else{
                        if(wallPositions[2][r][c+1]!=1 && wallPositions[2][r-1][c+1]!=1 && c <9){
                            legalMoves[0][r-1][c+1]=1;
                            // cout << "Case2: Move 0 " << r-1 << " " << c+1 << " is legal" << endl;
                        }
                        if(wallPositions[2][r][c]!=1 && wallPositions[2][r-1][c]!=1 && c >1){
                            legalMoves[0][r-1][c-1]=1;
                            // cout << "Case2: Move 0 " << r-1 << " " << c << " is legal" << endl;
                        }

                    }
                }
                else{
                    if(wallPositions[2][r][c+1]!=1 && wallPositions[2][r-1][c+1]!=1 && c <9){
                        legalMoves[0][r-1][c+1]=1;
                        // cout << "Case2: Move 0 " << r-1 << " " << c+1 << " is legal" << endl;
                    }
                    if(wallPositions[2][r][c]!=1 && wallPositions[2][r-1][c]!=1 && c >1){
                        legalMoves[0][r-1][c-1]=1;
                        // cout << "Case2: Move 0 " << r-1 << " " << c << " is legal" << endl;
                    }
                }
            }

        }   

    }
    ///cout << "r < 1" << endl;
    if(legalMoves[0][0][6] == 1){
        cout << "2 Invalid move legalized" << endl;
    }
    if(c<9){
        if (wallPositions[2][r][c+1] != 1 && wallPositions[2][r+1][c+1] != 1){
            if(!(r == orow && c+1 == ocol)){
                legalMoves[0][r][c+1]=1;
                // cout << "Case3: Move 0 " << r << " " << c+1 << " is legal" << endl;
            }
            else {
                if(wallPositions[2][r][c+2] != 1 && wallPositions[2][r+1][c+2] != 1 )
                {
                    if(c+1<9){
                        legalMoves[0][r][c+2]=1;
                        // cout << "Case3: Move 0 " << r << " " << c+2 << " is legal" << endl;
                    }
                    else{
                        if(wallPositions[1][r+1][c+1]!=1 && wallPositions[1][r+1][c+2]!=1 && r <9)
                        {
                            legalMoves[0][r+1][c+1]=1;
                            // cout << "Case3: Move 0 " << r+1 << " " << c+1 << " is legal" << endl;
                        }
                        if(wallPositions[1][r][c+1]!=1 && wallPositions[1][r][c+2]!=1 && r >1)
                        {
                            legalMoves[0][r-1][c+1]=1;
                            // cout << "Case3: Move 0 " << r << " " << c+1 << " is legal" << endl;
                        }

                    }
                }
                else{

                    if(wallPositions[1][r+1][c+1]!=1 && wallPositions[1][r+1][c+2]!=1 && r <9)
                    {
                        legalMoves[0][r+1][c+1]=1;
                        // cout << "Case3: Move 0 " << r+1 << " " << c+1 << " is legal" << endl;
                    }
                    if(wallPositions[1][r][c+1]!=1 && wallPositions[1][r][c+2]!=1 && r >1)
                    {
                        legalMoves[0][r-1][c+1]=1;
                        // cout << "Case3: Move 0 " << r << " " << c+1 << " is legal" << endl;
                    }
                }
            }
        }
    }
    //cout << "c < 9" << endl;
    if(legalMoves[0][0][6] == 1){
        cout << " 1 Invalid move legalized" << endl;
    }
    if(c>1)
    {
        if (wallPositions[2][r][c] != 1 && wallPositions[2][r+1][c] != 1 )
        {

            if(!(r == orow && c-1 == ocol)){
                legalMoves[0][r][c-1]=1;
                // cout << "Case4: Move 0 " << r << " " << c-1 << " is legal" << endl;
            }
            else {
                if(wallPositions[2][r][c-1] != 1 && wallPositions[2][r+1][c-1] != 1 )
                {
                    if(c-1>2)
                    {
                        legalMoves[0][r][c-2]=1;
                        // cout << "Case4: Move 0 " << r << " " << c-2 << " is legal" << endl;
                    }
                    else{
                        if(wallPositions[1][r+1][c]!=1 && wallPositions[1][r+1][c-1]!=1 && c <9)
                        {
                            legalMoves[0][r+1][c-1]=1;
                            // cout << "Case4: Move 0 " << r+1 << " " << c-1 << " is legal" << endl;
                        }
                        if(wallPositions[1][r][c]!=1 && wallPositions[1][r][c-1]!=1 && c-1 >2)
                        {
                            legalMoves[0][r-1][c-1]=1;
                            // cout << "Case4: Move 0 " << r << " " << c-1 << " is legal" << endl;
                        }

                    }
                }
                else{

                    if(wallPositions[1][r+1][c]!=1 && wallPositions[1][r+1][c-1]!=1 && c <9)
                    {
                        legalMoves[0][r+1][c-1]=1;
                        // cout << "Case4: Move 0 " << r+1 << " " << c-1 << " is legal" << endl;
                    }
                    if(wallPositions[1][r][c]!=1 && wallPositions[1][r][c-1]!=1 && c-1 >2)
                    {
                        legalMoves[0][r-1][c-1]=1;
                        // cout << "Case4: Move 0 " << r << " " << c-1 << " is legal" << endl;
                    }
                }
            }

        }
    }
    if(legalMoves[0][0][6] == 1){
        cout << "Invalid move legalized" << endl;
    }
    //cout << "c > 1" << endl;
    // cout << "##################################################" << endl;
    // cout << "##################################################" << endl;

}


void printHorWall(Node n){
    for(int j = 0;j<10;j++){
        for(int k = 0;k<10;k++){
            if(n.wallPositions[1][j][k] == 1){
                cout << "Wall 1 " << j << " " << k << " is there" << endl;
            }
        }
    }
}

void printVerWall(Node n){
    for(int j = 0;j<10;j++){
        for(int k = 0;k<10;k++){
            if(n.wallPositions[2][j][k] == 1){
                cout << "Wall 2 " << j << " " << k << " is there" << endl;
            }
        }
    }
}

int calcUtility(Node toReturn){
     if(player == 1){ //// ashish change
        return  3*get_r(toReturn.selfPos) - 5*(shortest_path_length(toReturn.selfPos,toReturn.wallPositions,1)) + 10*(shortest_path_length(toReturn.oppPos,toReturn.wallPositions,2));            
    }
    else{
        return 3*(10 - get_r(toReturn.selfPos))- 5*(shortest_path_length(toReturn.selfPos,toReturn.wallPositions,2)) + 10*(shortest_path_length(toReturn.oppPos,toReturn.wallPositions,1));
    }
    return 0;
}

Node newState(Node n, tuple<int,int,int> move,int playerNum,bool oppWon){
    int selfPos;
    int oppPos;
    int utility;
    // int numLegalMoves;
    int legalMoves[3][10][10];
    int oppLegalMoves[3][10][10];
    int wallPositions[3][11][11];
    int selfWalls = n.selfWalls;
    int oppWalls = n.oppWalls;
    int graph[82][82];

    ///Make this change upar
    for(int i = 0;i<3;i++){
        for(int j = 0;j<10;j++){
            for(int k = 0;k<10;k++){
                legalMoves[i][j][k] = 0;
                oppLegalMoves[i][j][k] = 0;
                // wallPositions[i][j][k] = n.wallPositions[i][j][k];
            }
        }
    }

    for(int i = 1;i<3;i++){
        for(int j = 0;j<11;j++){
            for(int k = 0;k<11;k++){
                wallPositions[i][j][k] = n.wallPositions[i][j][k];
            }
        }
    }

    int m = get<0>(move);
    int r = get<1>(move);
    int c = get<2>(move);

    selfPos = n.selfPos;
    oppPos = n.oppPos;

    int selfRow = get_r(selfPos);
    int selfCol = get_c(selfPos);

    int oppRow = get_r(oppPos);
    int oppCol = get_c(oppPos);

    

    if(m == 0){
        selfWalls = n.selfWalls;
        oppWalls = n.oppWalls;
        if(playerNum == player){////ashish change
            if(!iWon){
                selfPos = boxMapping(r,c);
            }
            
        }
        else{
            if(!oppWon){
                oppPos = boxMapping(r,c);
                if(oppPos == -3){
                    cout << "Problem @@ " << r << " " << c << "player " << playerNum << endl;
                }
            }
            else{
                oppPos=boxMapping(1,0);
            }
        }
    }
    else{
        wallPositions[m][r][c] = 1;

        if(player == playerNum){
            selfWalls -= 1;
        }
        else{
            oppWalls -= 1;
        }

        // TwoEdges te = getTwoEdges(m,r,c);
        // graph[te.p][te.q] = 0;
        // graph[te.q][te.p] = 0;
        // graph[te.r][te.s] = 0;
        // graph[te.s][te.r] = 0;

    }

    //**********************************************************//
    // if(m == 1){
    //     if((r == selfRow) && ( c == selfCol || c == selfCol + 1 )){
    //         legalMoves[0][r][c-1] 0;
    //         legalMoves[1]
    //     }
    // }
    //**********************************************************//

    Node toReturn(selfPos,oppPos,utility,legalMoves,oppLegalMoves,wallPositions,selfWalls,oppWalls,graph);
    //cout << "start making legal moves" << endl;
    setLegalMoves(toReturn.legalMoves,toReturn.wallPositions,toReturn.selfPos,toReturn.oppPos,player,selfWalls,oppWalls);
    setLegalMoves(toReturn.oppLegalMoves,toReturn.wallPositions,toReturn.oppPos,toReturn.selfPos,oppNum,oppWalls,selfWalls);

    // cout << "############____NEWSTATE_________#########" << endl;
    // cout << "Player " << playerNum << endl;
    // printLegalMoves(toReturn);
    // // printHorWall(toReturn);
    // // printVerWall(toReturn);
    // cout << " selfPos = " << selfPos << endl;
    // cout << " oppPos = " << oppPos << endl;
    // cout << "############______NEWSTATE DONE______#########" << endl;
    //cout << "start computing utility" << endl;

    
    // cout << "START #########_____________############_____________###########" << endl;
    // for(int i = 0; i < 10; i++){
    //     for(int j = 0; j < 10; j++){
    //         cout << wallPositions[1][i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // for(int i = 0; i < 10; i++){
    //     for(int j = 0; j < 10; j++){
    //         cout << wallPositions[2][i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << "END #########_____________############_____________###########" << endl;

    return toReturn;

}



int alpha_beta_pruning(Node n, int depth,int alpha,int beta,bool isSelf,int oppWon){
    int bestValue,temp;
    if(depth == 0){
        return calcUtility(n);
    }
    if(isSelf){
        bestValue = -1*INT_MAX;
        bool prune = false;
        for(int m = 0;m < 3;m++){
            if(prune){
                break;
            }
            for(int r = 0;r<10;r++){
                if(prune){
                    break;
                }
                for(int c = 0; c < 10; c++ ){
                    if(n.legalMoves[m][r][c] == 1){
                        Node tempChild = newState(n,make_tuple(m,r,c),player,oppWon);
                        temp = alpha_beta_pruning(tempChild,depth-1,alpha,beta,false,oppWon);
                        bestValue = max(temp,bestValue);
                        alpha = max(alpha,bestValue);
                        if(alpha >= beta){
                            prune = true;
                            break;
                        }
                    }
                }
            }
        }
        return bestValue;
    }
    else{
        bestValue = INT_MAX;
        bool prune = false;
        for(int m = 0;m < 3;m++){
            if(prune){
                break;
            }
            for(int r = 0;r<10;r++){
                if(prune){
                    break;
                }
                for(int c = 0; c < 10; c++ ){
                    if(n.oppLegalMoves[m][r][c] == 1){
                        Node tempChild = newState(n,make_tuple(m,r,c),oppNum,oppWon); //oppwon take care
                        temp = alpha_beta_pruning(tempChild,depth-1,alpha,beta,true,oppWon);
                        bestValue = min(temp,bestValue);
                        beta = min(beta,bestValue);
                        if(alpha >= beta){
                            prune = true;
                            break;
                        }
                    }
                }
            }
        }
        return bestValue;
    }
}



tuple<int,int,int> minimax(Node n,int depth,int oppWon,tuple<int,int,int> bestMove){
    int i=0;
    int j=0;
    int k=0;
    int bestValue = -1*INT_MAX;
    int temp;
    int bm = get<0>(bestMove);
    int br = get<1>(bestMove);
    int bc = get<2>(bestMove);
    n.legalMoves[bm][br][bc]=0;
    ///Only movements allowed till now
    for(int m = 0;m < 3;m++){
        for(int r = 0;r<10;r++){
            for(int c = 0; c < 10; c++ ){
                if(n.legalMoves[m][r][c] == 1){ 
                    // if( m == 2  && r == 9 && c == 4){cout << "start generating new " << endl;}
                    Node tempChild = newState(n,make_tuple(m,r,c),player,oppWon);
                    if(isGoal(tempChild.selfPos,player)){
                        i = m;
                        j = r;
                        k = c;
                        return make_tuple(i,j,k);
                    }
                    // if( m == 2  && r == 9 && c == 4){cout << "start pruning" << endl;}
                    temp = alpha_beta_pruning(tempChild,depth-1,-1*INT_MAX,INT_MAX,false,oppWon);
                    // if( m == 2  && r == 9 && c == 4){cout << "ended pruning" << endl;}
                    if(temp > bestValue){
                        bestValue = temp;
                        i = m;
                        j = r;
                        k = c;
                    }
                }
                // cout<<" C :  "<< c <<" done "<<endl; 
            }
            // cout<<" R :  "<< r <<" done "<<endl; 
        }
        // cout<<"Step "<< m <<" completed "<<endl; 
    }
    if(i==0 && j==0 && k==0){
        return make_tuple(bm,br,bc);
    }
    return make_tuple(i,j,k);
}

Node initialState(){
    int selfPos,oppPos;
    if(player == 1){
     selfPos = 5;
     oppPos = 77;
 }
 else{
     selfPos = 77;
     oppPos = 5;
 }

 int utility = 8;
 int legalMoves[3][10][10];
 int oppLegalMoves[3][10][10];
 int wallPositions[3][11][11];
 int selfWalls = 10;
 int oppWalls = 10;
    ///Change no. of walls
 int graph[82][82];

    ///Make this change upar
 for(int i = 0;i<3;i++){
    for(int j = 0;j<10;j++){
        for(int k = 0;k<10;k++){
            legalMoves[i][j][k] = 0;
            oppLegalMoves[i][j][k] = 0;
            // wallPositions[i][j][k] = 0;
        }
    }
}

for(int i = 0;i<3;i++){
    for(int j = 0;j<11;j++){
        for(int k = 0;k<11;k++){

            wallPositions[i][j][k] = 0;
        }
    }
}

for(int i = 0;i<82;i++){
    for(int j = 0;j<82;j++){
        graph[i][j] = 0;
        int r1 = get_r(i);
        int c1 = get_c(i);
        int r2 = get_r(j);
        int c2 = get_c(j);
        if((abs(r1-r2) == 1 && c1 == c2) || (abs(c1-c2) == 1 && r1 == r2)){
            graph[i][j] = 1;
        }
    }
}

setLegalMoves(legalMoves,wallPositions,selfPos,oppPos,player,selfWalls,oppWalls);
setLegalMoves(oppLegalMoves,wallPositions,oppPos,selfPos,oppNum,oppWalls,selfWalls);

Node initialState(selfPos,oppPos,utility,legalMoves,oppLegalMoves,wallPositions,selfWalls,oppWalls,graph);
return initialState;
}

void updateHistory(int history[4],int Pos){
    history[3]=history[2];
    history[2]=history[1];
    history[1]=history[0];
    history[0]=Pos;
}

// void merge(Node *a, int low, int high, int mid)
// {
//     int i, j, k;
//     int c[high-low+1];

//     i = low;
//     k = 0;
//     j = mid + 1;
//     while (i <= mid && j <= high)
//     {
//         if (a[i].utility < a[j].utility)
//         {
//             c[k] = a[i].utility;
//             k++;
//             i++;
//         }
//         else
//         {
//             c[k]= a[j].utility;
//             k++;
//             j++;
//         }
//     }
//     while (i <= mid)
//     {
//         c[k] = a[i].utility;
//         k++;
//         i++;
//     }
//     while (j <= high)
//     {
//         c[k] = a[j].utility;
//         k++;
//         j++;
//     }
//     for (i = low; i < k+low; i++)
//     {
//         a[i].utility = c[i-low];
//     }
// }
// void mergesort(Node *a, int low, int high)
// {
//     int mid;
//     if (low < high)
//     {
//         mid=(low+high)/2;
//         mergesort(a,low,mid);
//         mergesort(a,mid+1,high);
//         merge(a,low,high,mid);
//     }
//     return;
// }

void merge(int (*a)[2], int low, int high, int mid)
{
    int i, j, k, c[high-low+1][2];
    i = low;
    k = 0;
    j = mid + 1;
    while (i <= mid && j <= high)
    {
        if (a[i][1] < a[j][1])
        {
            c[k][1] = a[i][1];
            c[k][0] = a[i][0];
            k++;
            i++;
        }
        else
        {
            c[k][1] = a[j][1];
            c[k][0] = a[j][0];
            k++;
            j++;
        }
    }
    while (i <= mid)
    {
        c[k][1] = a[i][1];
        c[k][0] = a[i][0];
        k++;
        i++;
    }
    while (j <= high)
    {
        c[k][1] = a[j][1];
        c[k][0] = a[j][0];
        k++;
        j++;
    }

    for (i = low; i < k+low; i++)
    {
        a[i][1] = c[i-low][1];
        a[i][0] = c[i-low][0];
    }
    
}
void mergesort(int (*a)[2], int low, int high)
{
    int mid;
    if (low < high)
    {
        mid=(low+high)/2;
        mergesort(a,low,mid);
        mergesort(a,mid+1,high);
        merge(a,low,high,mid);
    }
    return;
}
void sortNodes(Node* node,int size){
    int utility[size][2];
    for(int i=0;i<size;i++){
        utility[i][0]=i;
        utility[i][1]=node[i].utility;
    }
    mergesort(utility,0,size-1);
}
int main(int argc, char *argv[]){
    srand (time(NULL));
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    char sendBuff[1025];
    struct sockaddr_in serv_addr; 
    int history[4];
    int numMoves=0;
    for (int i=0;i<4;i++)
    {
        history[i]=0;
    }

    

    if(argc != 3){
        printf("\n Usage: %s <ip of server> <port no> \n",argv[0]);
        return 1;
    } 


    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2])); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0){
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\n Error : Connect Failed \n");
        return 1;
    } 

    cout<<"Quoridor will start..."<<endl;
    // cout << "sizeof = " << sizeof(recvBuff) << endl;

    memset(recvBuff, '0',sizeof(recvBuff));
    n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
    recvBuff[n] = 0;
    sscanf(recvBuff, "%d %d %d %d %d", &player, &N, &M, &K, &time_left);
    oppNum = calcOppNumber(player);

    Node toSend = initialState();
    //printLegalMoves(toSend);

    cout<<"Player "<<player<<endl;
    cout<<"Time "<<time_left<<endl;
    cout<<"Board size "<<N<<"x"<<M<<" :"<<K<<endl;
    float TL;
    int om,oro,oc;
    int m,r,c;
    int d=3;
    bool oppWon = false;
    char s[100];
    int x=1;
    numLevels = 4;
    radius = 3;
    if(player == 1){
        memset(sendBuff, '0', sizeof(sendBuff)); 
        string temp;
        tuple<int,int,int> bestMove = minimax(toSend,1,oppWon,make_tuple(0,0,0));
        int m = get<0>(bestMove);
        int r = get<1>(bestMove);
        int c = get<2>(bestMove);
        int check_history=boxMapping(r,c);
        if(check_history== history[1]  && check_history ==  history[3] && history[0]==history[2] ){
            bestMove = minimax(toSend,1,oppWon,bestMove);
            m = get<0>(bestMove);
            r = get<1>(bestMove);
            c = get<2>(bestMove);   
        }

        updateHistory(history,boxMapping(r,c));
        


        // int m = 0;
        // int r = 2;
        // int c = 5;
        toSend = newState(toSend,make_tuple(m,r,c),player,oppWon);
        snprintf(sendBuff, sizeof(sendBuff), "%d %d %d", m, r , c);
        write(sockfd, sendBuff, strlen(sendBuff));

        memset(recvBuff, '0',sizeof(recvBuff));
        n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
        recvBuff[n] = 0;
        sscanf(recvBuff, "%f %d", &TL, &d);
        cout<<TL<<" "<<d<<endl;
        if(d==1){
            cout<<"You win!! Yayee!! :D ";
            x=0;
        }
        else if(d==2){
            cout<<"Loser :P ";
            x=0;
        }
        numMoves++;
    }
    ///set depth as per time
    ///handle player 2 as yourself
    while(x)
    {
        memset(recvBuff, '0',sizeof(recvBuff));
        n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
        recvBuff[n] = 0;
        sscanf(recvBuff, "%d %d %d %d", &om,&oro,&oc,&d);
        cout << om<<" "<<oro<<" "<<oc << " "<<d<<endl;
        if(d==1){
            cout<<"You win!! Yayee!! :D ";
            break;
        }
        else if(d==2){
            cout<<"Loser :P ";
            break;
        }
        else if(d == 31){ ////ashish change
            if(player!=1){
                oppWon = true;                
            }
            else{
                iWon=true;
            }
            
        }
        else if(d==32){
            if(player!=2){
                oppWon = true;                
            }
            else{
                iWon=true;
            }
        }
        memset(sendBuff, '0', sizeof(sendBuff)); 
        string temp;
        toSend = newState(toSend,make_tuple(om,oro,oc),oppNum,oppWon);
        //printLegalMoves(toSend);
        //printLegalWalls(toSend);
        cout << "selfPos = " << toSend.selfPos << " oppPos = " << toSend.oppPos << endl;
        //printHorWall(toSend);
        //printVerWall(toSend);
        cout << "Figuring out Min Path length = " << shortest_path_length(toSend.selfPos,toSend.wallPositions,player)<<endl;

        tuple<int,int,int> bestMove;
        if(numMoves<5){
            bestMove = minimax(toSend,1,oppWon,make_tuple(0,0,0));
        }
        else if(toSend.selfWalls + toSend.oppWalls < 4 && numLevels >= 3){
            bestMove = minimax(toSend,6,oppWon,make_tuple(0,0,0));
        }
        else if(toSend.selfWalls + toSend.oppWalls < 6 && numLevels >= 3){
            bestMove = minimax(toSend,5,oppWon,make_tuple(0,0,0));
        }
        else{

            bestMove = minimax(toSend,numLevels,oppWon,make_tuple(0,0,0));
        }
        
        int m = get<0>(bestMove);
        int r = get<1>(bestMove);
        int c = get<2>(bestMove);
        int check_history=boxMapping(r,c);
        cout<< "history  : " <<check_history<<" "<<history[0]<<" "<<history[1]<<" "<<history[2]<<" "<<history[3]<<endl;
        if(check_history== history[1]  && check_history ==  history[3] && history[0]==history[2]){
            cout<<"m r c : "<< m<<" "<<r << " "<<c << endl;
            bestMove = minimax(toSend,numLevels,oppWon,bestMove);
            m = get<0>(bestMove);
            r = get<1>(bestMove);
            c = get<2>(bestMove);
            cout<<"m r c updated : "<< m<<" "<<r << " "<<c << endl;   
        }
        updateHistory(history,boxMapping(r,c));
        cout << "trying move " << m << " " << r << " " << c << endl;
        toSend = newState(toSend,make_tuple(m,r,c),player,oppWon);

        // cin>>m>>r>>c;
        cout<<"Num moves done: " <<numMoves<<endl;
        numMoves++;
        snprintf(sendBuff, sizeof(sendBuff), "%d %d %d", m, r , c);
        write(sockfd, sendBuff, strlen(sendBuff));

        memset(recvBuff, '0',sizeof(recvBuff));
        n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
        recvBuff[n] = 0;
        sscanf(recvBuff, "%f %d", &TL, &d);//d=3 indicates game continues.. d=2 indicates lost game, d=1 means game won.
        cout<<TL<<" "<<d<<endl;
        if(d==1){
            cout<<"You win!! Yayee!! :D ";
            break;
        }
        else if(d==2){
            cout<<"Loser :P ";
            break;
        }
        else if(d == 31){ ////ashish change
            if(player!=1){
                oppWon = true;                
            }
            else{
                iWon=true;
            }
            
        }
        else if(d==32){
            if(player!=2){
                oppWon = true;                
            }
            else{
                iWon=true;
            }
        }
        if(TL<5){
            radius=2;
            numLevels=1;
        }
        else if(TL<10){
            radius=3;
            numLevels=2;
        }
        else if(TL<30){
            radius=3;
            numLevels=3;
        }
    }
    cout<<endl<<"The End"<<endl;
    return 0;
}

/// oppWon to be set
/// utility fn can be changed to shortest path when oppWon
/// utility fn can be changed to shortest path when oppWon , when less time or when oscillating
/// 5*path  player minimax=1
