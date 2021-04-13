#include <iostream>
#include <cstring>
#include <map>
#include <array>
#include <time.h>
#include <string.h>

#include "antlr4-runtime.h"
#include "XMLLexer.h"
#include "XMLParser.h"
#include "XMLParserBaseVisitor.h"
#include "XMLParserSecondVisitor.h"
#include <dirent.h>

using namespace antlr4;
using namespace std;


extern "C" void parse_to_pool(char* entry, size_t len);
extern "C" int parse(char* target,size_t len,char* second,size_t lenS);
extern "C" void fuzz(int index, char** ret, size_t* retlen);

#define MAXSAMPLES 10000
#define MAXTEXT 200
string ret[MAXSAMPLES];

const array<string, 8> types {"document", "prolog", "content", "element", "reference", "attribute", "chardata", "misc"};

class Subtree {
public:
	string type;
	array<string, 4> context;
	string text;
	Subtree(string ty, array<string, 4> c, string tex);
	~Subtree();
};






void WriteLog (const char* szLog) {
    /* get system time */
    time_t tt;
    time( &tt );
    tt = tt + 8*3600;  // transform the time zone
    tm* t= gmtime( &tt );

    /* write log info */
    FILE *fp;
    fp=fopen("/root/log.txt","at");
    fprintf(fp,"[Log Info: %d-%02d-%02d %02d:%02d]\t",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min
    );
    fprintf(fp, szLog);
    fclose(fp);
}

void parse_to_pool(char* entry, size_t len) {
    /* parse an entry(AST) into subtrees, and add they to the subtree pool */
    vector <misc::Interval> intervals;
    vector <string> texts;
    int num_of_samples = 0;

    // parse the entry
    string entry_string;
    try {
        entry_string = string(entry, len);
        ANTLRInputStream input(entry_string);
        XMLLexer lexer(&input);
        CommonTokenStream tokens(&lexer);
        XMLParser parser(&tokens);
        TokenStreamRewriter rewriter(&tokens);
        tree::ParseTree* tree = parser.document();
        if (parser.getNumberOfSyntaxErrors() > 0) {
            std::cerr<<"NumberOfSyntaxErrors:"<<parser.getNumberOfSyntaxErrors()<<endl;
            //return 0;
        } else {
            XMLParserBaseVisitor *visitor = new XMLParserBaseVisitor();
            visitor -> visit(tree);

            int interval_size = visitor->intervals.size();
            for (int i=0; i<interval_size; i++) {
                if (find(intervals.begin(), intervals.end(), visitor->intervals[i]) != intervals.end()) {
                } else if (visitor->intervals[i].a <= visitor->intervals[i].b) {
                    intervals.push_back(visitor->intervals[i]);
                }
            }

            int text_size = visitor->texts.size();
            for (int i=0; i<text_size; i++) {
                if (find(texts.begin(), texts.end(), visitor->texts[i]) != texts.end()) {
                } else if (visitor->texts[i].length()>MAXTEXT) {
                } else {
                    texts.push_back(visitor->texts[i]);
                }
            }

			// log writing
        	string s = "intervals:\n";
			for (int i=0; i<interval_size; i++) {
				s += intervals[i].toString();
				s += "\n";
			}
			s += "texts:\n";
			for (int i=0; i<text_size; i++) {
				s += texts[i];
				s += "\n";
			}
			WriteLog(s.c_str());
			// log writing




           	delete visitor;

        }
    } catch(range_error e){
		//std::cerr<<"range_error:"<<target<<endl;
	}
}


int parse(char* target,size_t len,char* second,size_t lenS) {
	/* 	target, second: test case
		len, lenS: size of test case */
	vector<misc::Interval> intervals;
	vector<string> texts;
	int num_of_smaples=0;
	//parse the target
	string targetString;
	try{
		targetString=string(target,len);
		ANTLRInputStream input(targetString);
		//ANTLRInputStream input(target);
		XMLLexer lexer(&input);
		CommonTokenStream tokens(&lexer);
		XMLParser parser(&tokens);
		TokenStreamRewriter rewriter(&tokens);
		tree::ParseTree* tree = parser.document();
				//cout<<targetString<<endl;
		if(parser.getNumberOfSyntaxErrors()>0){
			std::cerr<<"NumberOfSyntaxErrors:"<<parser.getNumberOfSyntaxErrors()<<endl;
			return 0;
		}else{
 
			XMLParserBaseVisitor *visitor=new XMLParserBaseVisitor();
			visitor->visit(tree);

			int interval_size = visitor->intervals.size();
			for(int i=0;i<interval_size;i++){
				if(find(intervals.begin(),intervals.end(),visitor->intervals[i])!=intervals.end()){
				}else if(visitor->intervals[i].a<=visitor->intervals[i].b){
					intervals.push_back(visitor->intervals[i]);
				}
			}
			int texts_size = visitor->texts.size();
			for(int i=0;i<texts_size;i++){
				if(find(texts.begin(),texts.end(),visitor->texts[i])!=texts.end()){
				}else if(visitor->texts[i].length()>MAXTEXT){
				}else{
					texts.push_back(visitor->texts[i]);
        			}
			}
            		delete visitor;

			//parse sencond
			string secondString;
			try{
				secondString=string(second,lenS);
				ANTLRInputStream inputS(secondString);
				XMLLexer lexerS(&inputS);
				CommonTokenStream tokensS(&lexerS);
				XMLParser parserS(&tokensS);
				tree::ParseTree* treeS = parserS.document();

				if(parserS.getNumberOfSyntaxErrors()>0){
		 			//std::cerr<<"NumberOfSyntaxErrors S:"<<parserS.getNumberOfSyntaxErrors()<<endl;
				}else{
					XMLParserSecondVisitor *visitorS=new XMLParserSecondVisitor();
					visitorS->visit(treeS);
					texts_size = visitorS->texts.size();
					for(int i=0;i<texts_size;i++){
						if(find(texts.begin(),texts.end(),visitorS->texts[i])!=texts.end()){
                        			}else if(visitorS->texts[i].length()>MAXTEXT){
						}else{
							texts.push_back(visitorS->texts[i]);
						}
					}
          			delete visitorS;
				}

				interval_size = intervals.size();
				texts_size = texts.size();

				/* subtree replace */
				for(int i=0;i<interval_size;i++){
					for(int j=0;j<texts_size;j++){
					    // energy assign
					    //for(int k=0; k<)
						rewriter.replace(intervals[i].a,intervals[i].b,texts[j]);
						ret[num_of_smaples++]=rewriter.getText();
						if(num_of_smaples>MAXSAMPLES){
							break;
						}
					}
					if(num_of_smaples>MAXSAMPLES){
						break;
					}
				}
			}catch(range_error e){
				//std::cerr<<"range_error"<<second<<endl;
			}
		}
	}catch(range_error e){
		//std::cerr<<"range_error:"<<target<<endl;
	}

	return num_of_smaples;
}

void fuzz(int index, char** result, size_t* retlen){
  *retlen=ret[index].length();
  *result=strdup(ret[index].c_str());
  //result=(char*)malloc(retlen+1);
  //strcpy(result,ret[index].c_str());
}


/*int main(){
  	ifstream in;
	string target;
  	in.open("/home/b/jscout_test/f2/queue/id:000686,src:000563,op:tree,pos:0,+cov");
	while(in>>target){
	}
  	int len=sizeof(target);
  	char second[]="var y=Number(20);\n";
  	int lenS=sizeof(second);
  	int num_of_smaples=parse(target,len,second,lenS);
  	for(int i=0;i<num_of_smaples;i++){
     	char* retbuf=nullptr;
     	size_t retlen=0;
     	fuzz(i,&retbuf,&retlen);
     	cout<<retlen<<retbuf<<endl;
  	}
  	cout<<num_of_smaples<<endl;
}*/

/*void getFiles(string path, vector<string>& files){
	DIR *dir;
	struct dirent *dp;
	if((dir=opendir(path.c_str()))==NULL){
		cout<<"Error"<<errno<<path<<endl;
	}
	while((dp=readdir(dir))!=NULL){
		files.push_back(string(dp->d_name));
	}
	closedir(dir);
}

int main(){
	string filePath="/home/b/xml/";
	vector<string> files;

	getFiles(filePath,files);

	char str[30];
	int size=files.size();
	for(int i=0;i<size;i++){
		cout<<files[i].c_str()<<endl;
	}
}*/
