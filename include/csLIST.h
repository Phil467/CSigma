#ifndef CSLIST_H_INCLUDED
#define CSLIST_H_INCLUDED
#include <iostream>

template<class TYPE> struct csLIST_DATA
{
    int position;
    TYPE data;
};

template<class TYPE> struct csLIST_DATATAB
{
    int position;
    TYPE* data;
    int size;
};

template<class TYPE> struct csLIST_DATATABPART
{
    int position;
    TYPE* data;
    int begin;
    int size;
};

struct csTABCOORDS
{
    int position;
    int size;
};

struct csSEARCH_PARAMS
{
    int pos = -1, incr=0;
    void init()
    {
        pos = -1; incr=0;
    }
};

template<class TYPE> class csLIST
{
    private:
        TYPE*Array;
        int memSize, arraySize, inc;
    public:
        csLIST()
        {
            Array=0;
            memSize=0;
            arraySize=0;
            inc=2;
        }
        /*~csLIST()
        {
            free(Array);
            Array = 0;
        }*/
    int size()
    {
        return arraySize;
    }
    int*sizePtr()
    {
        return &arraySize;
    }
    TYPE & operator[](int i)
    {
        return Array[i];
    }
    TYPE* getData()
    {
        return Array;
    }
    void init(int memsize, int _inc=2)
    {
        Array=0;
        memSize=0;
        arraySize=0;
        inc=_inc;

        if(memsize>0)
        {   arraySize=0;
            Array = (TYPE*)malloc(memsize*sizeof(TYPE));
            if(!Array) std::cout<<"Memory allocation error!\n";
            else memSize=memsize;
        }
        else std::cout<<"The size most be upper than 0!\n";
    }
    void initForce(int memsize, int _inc=2)
    {
        Array=0;
        memSize=0;
        arraySize=0;
        inc=_inc;

        if(memsize>0)
        {   arraySize=memsize;
            int l;
            Array = (TYPE*)malloc((l = memsize+_inc)*sizeof(TYPE));
            if(!Array) std::cout<<"Memory allocation error!\n";
            else memSize = l;
        }
        else std::cout<<"The size most be upper than 0!\n";
    }
    void full(int size, TYPE data)
    {
        if(size>0)
        {
            Array = (TYPE*)malloc(size*sizeof(TYPE));
            if(!Array) std::cout<<"Memory allocation error!\n";
            else
            {
                memSize=size;
                for(int i=0; i<size; i++)
                {
                    Array[i] = data;
                }
                arraySize=size;
            }
        }
        else std::cout<<"The size most be upper than 0!\n";
    }
    void placeBack(TYPE data)
    {
        if(!memSize) init(1);
        if(arraySize>=memSize)
        {   memSize+=inc;

            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {
                free(Array);
            }
            else Array=tmp;
        }
        Array[arraySize++]=data;
    }
    void placeBack2(TYPE data, size_t memsz)
    {
        if(!memSize) init(1);
        if(arraySize>=memSize)
        {   memSize+=memsz;
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
                free(Array);
            else Array=tmp;
        }
        Array[arraySize++]=data;
    }
    void placeFront(TYPE data)
    {
        if(!memSize) init(1);
        if(arraySize>=memSize)
        {   memSize+=inc;
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;
        }

        for(int i=arraySize-1; i>=0; i--)
        {   Array[i+1]=Array[i];
        }
        Array[0]=data;
        arraySize++;
    }
    void placeAt(TYPE data, int position)
    {
        if(!memSize) init(1);
        if(position<arraySize)
        {   if(arraySize>=memSize)
            {   memSize+=inc;
                TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(Array);
                    std::cout<<"Memory allocation error!\n";
                }
                else Array=tmp;
            }
            
            for(int i=arraySize-1; i>=position; i--)
                    Array[i+1]=Array[i];

            Array[position]=data;
            arraySize++;
        }
        else
        {
            memSize+=position-arraySize+1;
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;

            /*for(int i=arraySize; i<position; i++)
                Array[i]=zero;*/

            Array[position]=data;
            arraySize = position +1;
            std::cout<<"Warning : Position larger than table size.\n";
        }
    }
    void placeTableBack(TYPE* data, int size)
    {
        if(!memSize) init(1);
        if(arraySize>=memSize-size)
        {
            //arraySize = arraySize|1; // si arraySize = 0, arraySize = 1;
            memSize = arraySize + size +1;
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;
        }
        if(arraySize == 0)
        {
            for(int i=0, j=0; i<size; i++, j++)
                Array[i]=data[j];
            arraySize+=size;
        }
        else
        {
            for(int i=arraySize-1; i>=arraySize; i--)
                Array[i+size]=Array[i];

            int sz=arraySize+size;
            for(int i=arraySize, j=0; i<sz; i++, j++)
                Array[i]=data[j];
            arraySize+=size;
        }
    }
    void placeTableAt(TYPE* data, int position, int size)
    {
        if(!memSize) init(1);
        if(position<=arraySize)
        {
            if(arraySize>=memSize-size)
            {
                //arraySize = arraySize|1; // si arraySize = 0, arraySize = 1;
                memSize = arraySize + size +1;
                TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(Array);
                    std::cout<<"Memory allocation error!\n";
                }
                else Array=tmp;
            }
            if(arraySize == 0)
            {
                for(int i=0, j=0; i<size; i++, j++)
                    Array[i]=data[j];
                arraySize+=size;
            }
            else
            {
                for(int i=arraySize-1; i>=position; i--)
                    Array[i+size]=Array[i];

                int sz=position+size;
                for(int i=position, j=0; i<sz; i++, j++)
                    Array[i]=data[j];
                arraySize+=size;
            }
        }
        else
        {
            if(!memSize) init(1);
            memSize+=position-arraySize+size+1;
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;

            /*for(int i=arraySize; i<position; i++)
                Array[i]=zero;*/

            int sz=position+size;
            for(int i=position; i<sz; i++)
                Array[i]=data[i-position];
            arraySize = position + size;
            std::cout<<"Warning : Position larger than table size.\n";
            std::cout<<"This could cause problems!\n";
        }
    }
    void placeTablePartAt(int position, TYPE* data, int begin, int size)
    {
        if(!memSize) init(1);
        if(position<=arraySize)
        {   if(arraySize>=memSize-size)
            {   memSize+=size+1;
                TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(Array);
                    std::cout<<"Memory allocation error!\n";
                }
                else Array=tmp;
            }
            for(int i=arraySize; i>=position; i--)
            {   Array[i+size]=Array[i];
            }
            int sz=position+size;
            for(int i=position, j=begin; i<sz; i++, j++)
                Array[i]=data[j];
            arraySize+=size;
        }
        else
        {
            memSize+=position-arraySize+size+1;
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;

            /*for(int i=arraySize; i<position; i++)
                Array[i]=zero;*/

            int sz=position+size, pos = begin-position;
            for(int i=position, j=begin; i<sz; i++, j++)
                Array[i]=data[j];
            arraySize = position + size;
            std::cout<<"Warning : Position larger than table size.\n";
            std::cout<<"This could cause problems!\n";
        }
    }

    void move(int from, int to)
    {
        TYPE data = Array[from];
        if(from < to)
        {
            for(int i=from+1; i<=to; i++)
                Array[i-1] = Array[i];
            Array[to] = data;
        }
        else if(from > to)
        {
            for(int i=from-1; i>=to; i--)
                Array[i+1] = Array[i];
            Array[to] = data;
        }
    }
    void duplicate(int src, int pos)
    {
        TYPE data = Array[src];
        placeAt(data, pos);
    }
    void moveCharPtr(int from, int to);
    void duplicatePointList(int from, int to);
    char* toString(int from, int to);
    wchar_t* toStringW(int from, int to);
    void endString();
    void endStringW();
    csLIST<char> toList(int id);
    csLIST<wchar_t> toListW(int id);
    csLIST<TYPE> breakList(int from, int to);
    /*{
        char*data = csAlloc<char>(strlen((const char*)Array[from])+1);
        sprintf(data,"%s",Array[from]);

        if(from < to)
        {
            for(int i=from+1; i<to; i++)
            {
                int j = i-1;
                free(Array[j]);
                Array[j] = csAlloc<char>(strlen((const char*)Array[i])+1);
                printf(Array[j],"%s",Array[i]);
            }
            free(Array[to]);
            Array[to] = data;
        }
        else if(from > to)
        {
            for(int i=from-1; i>to; i--)
            {
                int j = i+1;
                free(Array[j]);
                Array[j] = csAlloc<char>(strlen((const char*)Array[i])+1);
                printf(Array[j],"%s",(const char*)Array[i]);
            }
            free(Array[to]);
            Array[to] = data;
        }
    }*/
    int findFirst(TYPE data)
    {
        for(int i=0; i<arraySize; i++)
        {
            if(Array[i] == data)
            {
                return i;
            }
        }
        return -1;
    }
    int findNext(TYPE data, int pos)
    {
        for(int i=pos; i<arraySize; i++)
        {
            if(Array[i] == data)
            {
                return i;
            }
        }
        return arraySize;
    }
    int findLast(TYPE data)
    {
        for(int i=arraySize-1; i>=0; i--)
        {
            if(Array[i] == data)
            {
                return i;
            }
        }
        return -1;
    }
    
    void slightSearch(TYPE data, csSEARCH_PARAMS* sp)
    {
        if(sp->incr < arraySize || sp->incr != sp->pos)
        {
            if(Array[sp->incr] == data)
                sp->pos = sp->incr;
            sp->incr++;
        }
    }
    void permute(int i, int j)
    {
        TYPE data = Array[i];
        Array[i] = Array[j];
        Array[j] = data;
    }
    void deleteEnd()
    {
        memSize=arraySize--;
        TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
        if(!tmp)
        {   free(Array);
            std::cout<<"Memory allocation error!\n";
        }
        else Array=tmp;

    }
    void deleteEndSafe()
    {
        memSize=arraySize--;
        if(memSize == 0)
        {
            clear();
        }
        else
        {
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;
        }

    }
    void deleteBegin()
    {
        for(int i=0; i<arraySize-1; i++)
        {   Array[i]=Array[i+1];
        }
        memSize=--arraySize;
        TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
        if(!tmp)
        {   free(Array);
            std::cout<<"Memory allocation error!\n";
        }
        else Array=tmp;
    }
    void deleteAt(int position)
    {
        if(position<arraySize)
        {
            if(arraySize>1)
            {
                for(int i=position; i<arraySize-1; i++)
                Array[i]=Array[i+1];

                memSize=--arraySize;
                TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(Array);
                    std::cout<<"Memory allocation error!\n";
                }
                else Array=tmp;
            }
            else clear();
        }
        else std::cout<<"Position larger than table size!\n";
    }
    void deleteTableAtBegining(int size)
    {
        if(size<arraySize)
        {
            for(int i=size; i<arraySize; i++)
                Array[i-size]=Array[i];
            memSize=(arraySize-=size);
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;
        }
        else std::cout<<"Position larger than table size!\n";
    }
    void deleteTableAt(int position, int size)
    {
        int sz=position+size;
        if(sz<arraySize)
        {
            for(int i=sz; i<arraySize; i++)
                Array[i-size]=Array[i];

            memSize=(arraySize-=size);
            TYPE* tmp=(TYPE*)realloc(Array, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(Array);
                std::cout<<"Memory allocation error!\n";
            }
            else Array=tmp;
        }
        else
        {
            if(position==0&&size>=arraySize)
                clear();
            else
                std::cout<<"Position larger than table size!\n";
        }
    }
    void minMaxSortD()
    {
        if(arraySize>0)
        {
            double mn, mx;
            for(int j=0; j<arraySize; j++)
            for(int i=1; i<arraySize; i++)
            {
                mx=(double)Array[i];
                mn =(double)Array[i-1];
                if(mx<mn)
                {
                    Array[i]=(TYPE)mn;
                    Array[i-1]=(TYPE)mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void maxMinSortD()
    {
        if(arraySize>0)
        {
            double mn, mx;
            for(int j=0; j<arraySize; j++)
            for(int i=1; i<arraySize; i++)
            {
                mx=(double)Array[i];
                mn =(double)Array[i-1];
                if(mx>mn)
                {
                    Array[i]=(TYPE)mn;
                    Array[i-1]=(TYPE)mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void minMaxSortL()
    {
        if(arraySize>0)
        {
            long mn, mx;
            for(int j=0; j<arraySize; j++)
            for(int i=1; i<arraySize; i++)
            {
                mx = Array[i];
                mn = Array[i-1];
                if(mx<mn)
                {
                    Array[i]=mn;
                    Array[i-1]=mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void maxMinSortL()
    {
        if(arraySize>0)
        {
            long mn, mx;
            for(int j=0; j<arraySize; j++)
            for(int i=1; i<arraySize; i++)
            {
                mx = Array[i];
                mn = Array[i-1];
                if(mx>mn)
                {
                    Array[i]=mn;
                    Array[i-1]=mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void print(int i)
    {
        std::cout<<Array[i]<<" ";
    }
    void printD(int i)
    {
        //std::cout<<setprecision(precision)<<*(double*)&Array[i]<<" ";
        std::cout<<Array[i]<<" ";
    }
    void printAll(const char*title="")
    {
        std::cout<<title<<"\n{ ";
        for(int i=0; i<arraySize; i++)
            print(i);
        std::cout<<"}\n";
    }
    void printAllD(const char*title="")
    {
        std::cout<<title<<"\n{ ";
        for(int i=0; i<arraySize; i++)
            printD(i);
        std::cout<<"}\n";
    }
    void operator+=(TYPE data)
    {
        placeBack(data);
    }
    /*TYPE& operator+=(const TYPE data[], size_t n)
    {
        if(sizeof(data))
            placeTableAt(data, arraySize, sizeof(data)/sizeof(data[0]));
        return *this;
    }*/
    void operator+=(csLIST_DATA<TYPE> data)
    {
        placeAt(data.data,data.position);
    }
    void operator+=(csLIST_DATATAB<TYPE> data)
    {
        placeTableAt(data.data,data.position,data.size);
    }
    void operator+=(csLIST_DATATABPART<TYPE> data)
    {
        placeTablePartAt(data.position,data.data,data.begin,data.size);
    }
    void operator--(int i)
    {
        deleteEnd();
    }
    void operator-=(unsigned int i)
    {
        deleteAt(i);
    }
    void operator-=(csTABCOORDS coords)
    {
        deleteTableAt(coords.position, coords.size);
    }
    void clear()
    {
        if(arraySize)
        {
            free(Array);
            Array=NULL;
            arraySize=0;
            memSize=0;
        }
    }
};


#endif // CSLIST_H_INCLUDED
