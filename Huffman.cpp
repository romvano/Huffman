#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

struct CNode {
    unsigned char Sym;
    std::uint64_t Frequency;
    CNode* Left;
    CNode* Right;

    CNode(): Sym( 0 ), Frequency( 0 ), Left( 0 ), Right( 0 ) {}
    CNode( unsigned char sym, int frequency, CNode* Left = 0, CNode* Right = 0 ):
        Sym( sym ), Frequency( frequency ), Left( Left ), Right( Right ) {}
    bool canBeSymbol() const { return !( (bool) Left && (bool) Right ); }
    ~CNode() { delete Left; delete Right; }
};

bool frequencyCmp( const CNode* a, const CNode* b) {
    return a->Frequency < b->Frequency;
}

CNode* alphabet = 0;
CNode* getAlphabet( const std::string& source ) {
    // частоты появления символа i в строке
    std::vector< std::int64_t > frequency( std::numeric_limits< char >::max(), 0 );
    // подсчет частот
    for( std::uint64_t i = 0; i < source.size(); ++i ) {
        ++frequency[ source[ i ] ];
    }
    // вектор, в котором будет захимичен Хаффман
    std::vector< CNode* > tree;
    // в качестве приоритета используем частоту
    for( auto i = 0; i < frequency.size(); ++i ) {
        if( frequency[ i ] ) {
            tree.push_back( new CNode( i, frequency [ i ] ) );
        }
    }
    // сортировка по минимальному приоритету
    if( tree.size() > 1 ) {
        std::sort( tree.begin(), tree.end(), frequencyCmp );
        auto beginning = 0;
        while( beginning < tree.size() - 1 ) {
            CNode* temp = new CNode( 0,
                                     tree[ beginning ]->Frequency + tree[ beginning + 1 ]->Frequency,
                                     tree[ beginning ],
                                     tree[ beginning + 1 ]
            );
            tree[ ++beginning ] = temp;
            std::sort( tree.begin() + beginning, tree.end(), frequencyCmp );
        }
    }
    auto head = tree[ tree.size() - 1 ];
    return head;
}

void fillVector( std::vector< std::string >& vec, std::string result, CNode* node ) {
    if( node->canBeSymbol() )
        vec[ node->Sym ] = result;
    else {
        if( node->Left ) {
            fillVector( vec, result + '0', node->Left );
        }
        if( node->Right ) {
            fillVector( vec, result + '1', node->Right );
        }
    }
}

void compress_string( const std::string &source, std::string &compressed ) {
    compressed = "";
    if( !source.size() )
        return;
    alphabet = getAlphabet( source );
    std::vector< std::string > alphabetVector( std::numeric_limits< char >::max(), "" );
    std::string temp = alphabet->canBeSymbol() ? "0" : "";
    fillVector( alphabetVector, temp, alphabet );
//std::cout<<"vector:\n";
//for( unsigned char i = 0; i < alphabetVector.size(); ++i ) std::cout<<i<<'='<<alphabetVector[i]<<' ';
//std::cout<<std::endl;
    for( auto sym : source ) {
        compressed += alphabetVector[ sym ];
    }
}

void decompress_string( const std::string &compressed, std::string &result ) {
    result = "";
    if( !compressed.size() )
        return;
    if( alphabet->canBeSymbol() ) {
        for( std::uint64_t i = 0; i < compressed.size(); ++i ) {
            result += alphabet->Sym;
        }
    } else {
        for( std::uint64_t i = 0; i < compressed.size(); ) {
            CNode* node = alphabet;
            while( !node->canBeSymbol() ) {
                node = ( compressed[ i++ ] == '1' ) ? node->Right : node->Left;
            }
            result += node->Sym;
        }
    }
    delete alphabet;
}

int main( int argn, char** argv ) {
    std::string source;
    if( argn > 1 )
        source = argv[1];
    else 
        source = "aaa";
    std::string compressed;
    std::string result;

    compress_string( source, compressed );
    std::cout << compressed << std::endl;
    decompress_string( compressed, result );
    std::cout << result << std::endl;
    return 0;
}
