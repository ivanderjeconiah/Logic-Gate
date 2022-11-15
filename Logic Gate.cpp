#include <iostream>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

//input output
struct io{
    string input, output;
};

struct node{
    string name;
    //to check output
    vector<io> check;
    //to check output name
    vector<io> checkName;
};

class Kiss
{

    public:
    vector<node> varArray;

    int inVarArray(string name, vector<node> varArray){
        for(int i = 0; i < varArray.size(); i++){
            if(name == varArray[i].name) return i;
        }
        return -1;
    }

    int inTestArray(string a, string b, vector<io> testArray){
        for(int i = 0; i < testArray.size(); i++){
            if( (a == testArray[i].input && b == testArray[i].output ) || (b == testArray[i].input && a == testArray[i].output )){
                return i;
            }
        }
        return -1;
    }

    bool inVar(string name, vector<string> var){
        for(int i = 0; i < var.size(); i++){
            if(name == var[i]) return true;
        }
        return false;
    }

    bool sameOutput(node A, node B){
        for(int i = 0; i < A.check.size(); i++){
            for(int j = 0; j < B.check.size(); j++){
                if(A.check[i].input == B.check[j].input){
                    if(A.check[i].output != B.check[j].output) return false;
                }
            }
        }
        return true;
    }

};

int main(){

    ifstream file;
    file.open('input.kiss');
    cout<<file<<endl;
    if(!file) cout<<"File not found!"<<endl;
    string text, startingVariable;
    int cmdAMT = 0, varAMT = 0, inputAMT = 0, outputAMT = 0;

    file >> text;
    while(file >> text){
        if(text == ".i"){
            file >> inputAMT;
        }
        else if(text == ".o"){
            file >> outputAMT;;
        }
        else if(text == ".p"){
            file >> cmdAMT;
        }
        else if(text == ".s"){
            file >> varAMT;
        }
        else if(text == ".r"){
            file >> startingVariable;

            vector<io> testArray;
            vector<string> tempVar;

            Kiss state;

            //input command
            for(int i = 0; i < cmdAMT; i++){

                string varIn, varOut, output, input;
                file >> input >> varIn >> varOut >> output;

                if(!state.inVar(varIn, tempVar)) tempVar.push_back(varIn);

                int ind = state.inVarArray(varIn, state.varArray);
                io tempIO = {input, output}, tempIOName = {input, varOut};

                //old node
                if(ind >= 0){
                    state.varArray[ind].check.push_back(tempIO);
                    state.varArray[ind].checkName.push_back(tempIOName);
                }
                //new node
                else{
                    vector<io> tempVecIO, tempVecIOName;
                    tempVecIO.push_back(tempIO);
                    tempVecIOName.push_back(tempIOName);

                    node tempNode = {varIn, tempVecIO, tempVecIOName};
                    state.varArray.push_back(tempNode);
                }
            }

            //in.dot
            string fname = argv[1];
            fname.erase(fname.end()-4, fname.end());
            fname += "dot";
            ofstream in(fname);
            in << "digraph STG {" << endl;
            in << "\trankdir=LR;" << endl << endl;
            in << "\tINIT [shape=point];" <<endl;
            for(int i = 0; i < state.varArray.size(); i++) in << "\t" << state.varArray[i].name << " [label=\"" << state.varArray[i].name << "\"];" << endl;
            in<<endl;
            in<< "\tINIT -> " << startingVariable << ";" << endl;

            for(int i = 0; i < state.varArray.size(); i++){
                vector<string> replacement;
                for(int j = 0; j < 2 * inputAMT; j++){
                    if(!state.inVar(state.varArray[i].check[j].input, replacement)){
                        in << "\t" << state.varArray[i].name << " -> " << state.varArray[i].checkName[j].output << " [label=\"";
                        in << state.varArray[i].check[j].input << "/" << state.varArray[i].check[j].output;
                        for(int k = j + 1; k < 2 * inputAMT; k++){
                            if(state.varArray[i].checkName[j].output == state.varArray[i].checkName[k].output){
                                in << "," << state.varArray[i].check[k].input << "/" << state.varArray[i].check[k].output;
                                replacement.push_back(state.varArray[i].check[k].input);
                            }
                        }
                        in << "\"];" << endl;
                    }
                }
            }
            in << "}" << endl;
            in.close();

            //input testing array
            for(int i = 0; i < tempVar.size(); i++){
                for(int j = i + 1; j < tempVar.size(); j++){
                    testArray.push_back({tempVar[i], tempVar[j]});
                }
            }

            //find unsuited partners
            for(int i = 0; i < testArray.size(); i++){
                int indA = state.inVarArray(testArray[i].input, state.varArray);
                int indB = state.inVarArray(testArray[i].output, state.varArray);
                int eraseInd = -1;
                //check if same output

                if(!state.sameOutput(state.varArray[indA], state.varArray[indB])){
                    testArray.erase(testArray.begin() + i);
                    i--;
                }
                else{
                    int res = 0;
                    for(int j = 0; j < 2 * inputAMT; j++){
                        for(int k = 0; k < 2 * inputAMT; k++){
                            if(state.varArray[indA].checkName[j].input == state.varArray[indB].checkName[k].output){
                                int indC = state.inVarArray(state.varArray[indA].checkName[j].output, state.varArray);
                                int indD = state.inVarArray(state.varArray[indB].checkName[k].output, state.varArray);
                                if((indA == indC && indB == indD) || (indC == indB && indA == indD) || (indD == indC)) continue;
                                if(!state.sameOutput(state.varArray[indC], state.varArray[indD])){
                                    eraseInd = 1;
                                    res = 1;
                                }
                                break;
                            }
                        }
                        if(res == 1) break;
                    }
                }

                if(eraseInd >= 0){
                    testArray.erase(testArray.begin() + i);
                    i--;
                }
            }

            int repeat = 0;
            while(repeat == 0){
                repeat = 1;
                for(int i = 0; i < testArray.size(); i++){
                    int indA = state.inVarArray(testArray[i].input, state.varArray);
                    int indB = state.inVarArray(testArray[i].output, state.varArray);
                    int res = 0;
                    for(int j = 0; j < 2 * inputAMT; j++){
                        for(int k = 0; k < 2 * inputAMT; k++){
                            if(state.varArray[indA].checkName[j].input == state.varArray[indB].checkName[k].input){
                                //if output is same
                                if(state.varArray[indA].checkName[j].output == state.varArray[indB].checkName[k].output) continue;

                                //if output is same as input
                                if((state.varArray[indA].checkName[j].output == state.varArray[indA].name &&
                                   state.varArray[indB].checkName[k].output == state.varArray[indB].name) ||
                                   (state.varArray[indA].checkName[j].output == state.varArray[indB].name &&
                                   state.varArray[indB].checkName[k].output == state.varArray[indA].name)) continue;

                                int indX = state.inTestArray(state.varArray[indA].checkName[j].output, state.varArray[indB].checkName[k].output, testArray);
                                //if output not in array
                                if(indX == -1){
                                    testArray.erase(testArray.begin() + i);
                                    i--;
                                    repeat = 0;
                                    res = 1;
                                }
                                break;
                            }
                        }
                        if(res == 1) break;
                    }
                }
            }

            //update varArray
            for(int i = 0; i < testArray.size(); i++){
                for(int j = 0; j < state.varArray.size(); j++){
                    if(testArray[i].output == state.varArray[j].name){
                        state.varArray.erase(state.varArray.begin() + j);
                        j--;
                    }
                    else{
                        for(int k = 0; k < state.varArray[j].checkName.size(); k++){
                            if(testArray[i].output == state.varArray[j].checkName[k].output){
                                state.varArray[j].checkName[k].output = testArray[i].input;
                            }
                        }
                    }
                }
            }

            //out.kiss
            fname = argv[2];
            ofstream output(fname);
            output << ".start_kiss" << endl;
            output << ".i " << inputAMT << endl;
            output << ".o " << outputAMT << endl;
            output << ".p " << state.varArray.size() * 2 * inputAMT<< endl;
            output << ".s " << state.varArray.size() << endl;
            output << ".r " << startingVariable << endl;
            for(int i = 0; i < state.varArray.size(); i++){
                for(int j = 0; j < 2 * inputAMT; j++){
                    output << state.varArray[i].check[j].input << " " << state.varArray[i].name << " " << state.varArray[i].checkName[j].output << " " << state.varArray[i].check[j].output << endl;
                }
            }
            output << ".end_kiss" << endl;
            output.close();

            //out.dot
            fname = argv[3];
            ofstream out(fname);
            out << "digraph STG {" << endl;
            out << "\trankdir=LR;" << endl << endl;
            out << "\tINIT [shape=point];" <<endl;

            for(int i = 0; i < state.varArray.size(); i++) out << "\t" << state.varArray[i].name << " [label=\"" << state.varArray[i].name << "\"];" << endl;

            out<<endl;
            out<< "\tINIT -> " << startingVariable << ";" << endl;

            for(int i = 0; i < state.varArray.size(); i++){
                vector<string> replacement;
                for(int j = 0; j < 2 * inputAMT; j++){
                    if(!state.inVar(state.varArray[i].check[j].input, replacement)){
                        out << "\t" << state.varArray[i].name << " -> " << state.varArray[i].checkName[j].output << " [label=\"";
                        out << state.varArray[i].check[j].input << "/" << state.varArray[i].check[j].output;
                        for(int k = j + 1; k < 2 * inputAMT; k++){
                            if(state.varArray[i].checkName[j].output == state.varArray[i].checkName[k].output){
                                out << "," << state.varArray[i].check[k].input << "/" << state.varArray[i].check[k].output;
                                replacement.push_back(state.varArray[i].check[k].input);
                            }
                        }
                        out << "\"];" << endl;
                    }
                }
            }
            out << "}" << endl;
            out.close();
        }
        else break;
    }
}
