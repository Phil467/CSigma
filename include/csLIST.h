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
        TYPE*DataTab;
        int memSize, tabSize, inc;
    public:
        csLIST()
        {
            DataTab=0;
            memSize=0;
            tabSize=0;
            inc=2;
        }
        /*~csLIST()
        {
            free(DataTab);
            DataTab = 0;
        }*/
    int size()
    {
        return tabSize;
    }
    int*sizePtr()
    {
        return &tabSize;
    }
    TYPE & operator[](int i)
    {
        return DataTab[i];
    }
    TYPE* getTable()
    {
        return DataTab;
    }
    void init(int memsize, int _inc=2)
    {
        DataTab=0;
        memSize=0;
        tabSize=0;
        inc=_inc;

        if(memsize>0)
        {   tabSize=0;
            DataTab = (TYPE*)malloc(memsize*sizeof(TYPE));
            if(!DataTab) std::cout<<"Memory allocation error!\n";
            else memSize=memsize;
        }
        else std::cout<<"The size most be upper than 0!\n";
    }
    void initForce(int memsize, int _inc=2)
    {
        DataTab=0;
        memSize=0;
        tabSize=0;
        inc=_inc;

        if(memsize>0)
        {   tabSize=memsize;
            int l;
            DataTab = (TYPE*)malloc((l = memsize+_inc)*sizeof(TYPE));
            if(!DataTab) std::cout<<"Memory allocation error!\n";
            else memSize = l;
        }
        else std::cout<<"The size most be upper than 0!\n";
    }
    void full(int size, TYPE data)
    {
        if(size>0)
        {
            DataTab = (TYPE*)malloc(size*sizeof(TYPE));
            if(!DataTab) std::cout<<"Memory allocation error!\n";
            else
            {
                memSize=size;
                for(int i=0; i<size; i++)
                {
                    DataTab[i] = data;
                }
                tabSize=size;
            }
        }
        else std::cout<<"The size most be upper than 0!\n";
    }
    void insertEnd(TYPE data)
    {
        if(!memSize) init(1);
        if(tabSize>=memSize)
        {   memSize+=inc;

            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {
                free(DataTab);
            }
            else DataTab=tmp;
        }
        DataTab[tabSize++]=data;
    }
    void insertEnd2(TYPE data, size_t memsz)
    {
        if(!memSize) init(1);
        if(tabSize>=memSize)
        {   memSize+=memsz;
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
                free(DataTab);
            else DataTab=tmp;
        }
        DataTab[tabSize++]=data;
    }
    void insertBegin(TYPE data)
    {
        if(!memSize) init(1);
        if(tabSize>=memSize)
        {   memSize+=inc;
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;
        }

        for(int i=tabSize-1; i>=0; i--)
        {   DataTab[i+1]=DataTab[i];
        }
        DataTab[0]=data;
        tabSize++;
    }
    void insertAt(TYPE data, int position)
    {
        if(!memSize) init(1);
        if(position<=tabSize)
        {   if(tabSize>=memSize)
            {   memSize+=inc;
                TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(DataTab);
                    std::cout<<"Memory allocation error!\n";
                }
                else DataTab=tmp;
            }
            for(int i=tabSize-1; i>=position; i--)
                DataTab[i+1]=DataTab[i];

            DataTab[position]=data;
            tabSize++;
        }
        else
        {
            memSize+=position-tabSize+1;
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;

            /*for(int i=tabSize; i<position; i++)
                DataTab[i]=zero;*/

            DataTab[position]=data;
            tabSize = position +1;
            std::cout<<"Warning : Position larger than table size.\n";
            std::cout<<"This could cause problems!\n";
        }
    }
    void insertTableEnd(TYPE* data, int size)
    {
        if(!memSize) init(1);
        if(tabSize>=memSize-size)
        {
            //tabSize = tabSize|1; // si tabSize = 0, tabSize = 1;
            memSize = tabSize + size +1;
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;
        }
        if(tabSize == 0)
        {
            for(int i=0, j=0; i<size; i++, j++)
                DataTab[i]=data[j];
            tabSize+=size;
        }
        else
        {
            for(int i=tabSize-1; i>=tabSize; i--)
                DataTab[i+size]=DataTab[i];

            int sz=tabSize+size;
            for(int i=tabSize, j=0; i<sz; i++, j++)
                DataTab[i]=data[j];
            tabSize+=size;
        }
    }
    void insertTableAt(TYPE* data, int position, int size)
    {
        if(!memSize) init(1);
        if(position<=tabSize)
        {
            if(tabSize>=memSize-size)
            {
                //tabSize = tabSize|1; // si tabSize = 0, tabSize = 1;
                memSize = tabSize + size +1;
                TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(DataTab);
                    std::cout<<"Memory allocation error!\n";
                }
                else DataTab=tmp;
            }
            if(tabSize == 0)
            {
                for(int i=0, j=0; i<size; i++, j++)
                    DataTab[i]=data[j];
                tabSize+=size;
            }
            else
            {
                for(int i=tabSize-1; i>=position; i--)
                    DataTab[i+size]=DataTab[i];

                int sz=position+size;
                for(int i=position, j=0; i<sz; i++, j++)
                    DataTab[i]=data[j];
                tabSize+=size;
            }
        }
        else
        {
            if(!memSize) init(1);
            memSize+=position-tabSize+size+1;
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;

            /*for(int i=tabSize; i<position; i++)
                DataTab[i]=zero;*/

            int sz=position+size;
            for(int i=position; i<sz; i++)
                DataTab[i]=data[i-position];
            tabSize = position + size;
            std::cout<<"Warning : Position larger than table size.\n";
            std::cout<<"This could cause problems!\n";
        }
    }
    void insertTablePartAt(int position, TYPE* data, int begin, int size)
    {
        if(!memSize) init(1);
        if(position<=tabSize)
        {   if(tabSize>=memSize-size)
            {   memSize+=size+1;
                TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(DataTab);
                    std::cout<<"Memory allocation error!\n";
                }
                else DataTab=tmp;
            }
            for(int i=tabSize; i>=position; i--)
            {   DataTab[i+size]=DataTab[i];
            }
            int sz=position+size;
            for(int i=position, j=begin; i<sz; i++, j++)
                DataTab[i]=data[j];
            tabSize+=size;
        }
        else
        {
            memSize+=position-tabSize+size+1;
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;

            /*for(int i=tabSize; i<position; i++)
                DataTab[i]=zero;*/

            int sz=position+size, pos = begin-position;
            for(int i=position, j=begin; i<sz; i++, j++)
                DataTab[i]=data[j];
            tabSize = position + size;
            std::cout<<"Warning : Position larger than table size.\n";
            std::cout<<"This could cause problems!\n";
        }
    }

    void move(int from, int to)
    {
        TYPE data = DataTab[from];
        if(from < to)
        {
            for(int i=from+1; i<=to; i++)
                DataTab[i-1] = DataTab[i];
            DataTab[to] = data;
        }
        else if(from > to)
        {
            for(int i=from-1; i>=to; i--)
                DataTab[i+1] = DataTab[i];
            DataTab[to] = data;
        }
    }
    void duplicate(int src, int pos)
    {
        TYPE data = DataTab[src];
        insertAt(data, pos);
    }
    void moveCharPtr(int from, int to);
    void duplicatePointList(int from, int to);
    char* toString(int from, int to);
    wchar_t* toStringW(int from, int to);
    csLIST<char> toList(int id);
    csLIST<wchar_t> toListW(int id);
    csLIST<TYPE> breakList(int from, int to);
    /*{
        char*data = csAlloc<char>(strlen((const char*)DataTab[from])+1);
        sprintf(data,"%s",DataTab[from]);

        if(from < to)
        {
            for(int i=from+1; i<to; i++)
            {
                int j = i-1;
                free(DataTab[j]);
                DataTab[j] = csAlloc<char>(strlen((const char*)DataTab[i])+1);
                printf(DataTab[j],"%s",DataTab[i]);
            }
            free(DataTab[to]);
            DataTab[to] = data;
        }
        else if(from > to)
        {
            for(int i=from-1; i>to; i--)
            {
                int j = i+1;
                free(DataTab[j]);
                DataTab[j] = csAlloc<char>(strlen((const char*)DataTab[i])+1);
                printf(DataTab[j],"%s",(const char*)DataTab[i]);
            }
            free(DataTab[to]);
            DataTab[to] = data;
        }
    }*/
    int findFirst(TYPE data)
    {
        for(int i=0; i<tabSize; i++)
        {
            if(DataTab[i] == data)
            {
                return i;
            }
        }
        return -1;
    }
    int findNext(TYPE data, int pos)
    {
        for(int i=pos; i<tabSize; i++)
        {
            if(DataTab[i] == data)
            {
                return i;
            }
        }
        return tabSize;
    }
    int findLast(TYPE data)
    {
        for(int i=tabSize-1; i>=0; i--)
        {
            if(DataTab[i] == data)
            {
                return i;
            }
        }
        return -1;
    }
    
    void slightSearch(TYPE data, csSEARCH_PARAMS* sp)
    {
        if(sp->incr < tabSize || sp->incr != sp->pos)
        {
            if(DataTab[sp->incr] == data)
                sp->pos = sp->incr;
            sp->incr++;
        }
    }
    void permute(int i, int j)
    {
        TYPE data = DataTab[i];
        DataTab[i] = DataTab[j];
        DataTab[j] = data;
    }
    void deleteEnd()
    {
        memSize=tabSize--;
        TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
        if(!tmp)
        {   free(DataTab);
            std::cout<<"Memory allocation error!\n";
        }
        else DataTab=tmp;

    }
    void deleteEndSafe()
    {
        memSize=tabSize--;
        if(memSize == 0)
        {
            clear();
        }
        else
        {
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;
        }

    }
    void deleteBegin()
    {
        for(int i=0; i<tabSize-1; i++)
        {   DataTab[i]=DataTab[i+1];
        }
        memSize=--tabSize;
        TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
        if(!tmp)
        {   free(DataTab);
            std::cout<<"Memory allocation error!\n";
        }
        else DataTab=tmp;
    }
    void deleteAt(int position)
    {
        if(position<tabSize)
        {
            if(tabSize>1)
            {
                for(int i=position; i<tabSize-1; i++)
                DataTab[i]=DataTab[i+1];

                memSize=--tabSize;
                TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
                if(!tmp)
                {   free(DataTab);
                    std::cout<<"Memory allocation error!\n";
                }
                else DataTab=tmp;
            }
            else clear();
        }
        else std::cout<<"Position larger than table size!\n";
    }
    void deleteTableAtBegining(int size)
    {
        if(size<tabSize)
        {
            for(int i=size; i<tabSize; i++)
                DataTab[i-size]=DataTab[i];
            memSize=(tabSize-=size);
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;
        }
        else std::cout<<"Position larger than table size!\n";
    }
    void deleteTableAt(int position, int size)
    {
        int sz=position+size;
        if(sz<tabSize)
        {
            for(int i=sz; i<tabSize; i++)
                DataTab[i-size]=DataTab[i];

            memSize=(tabSize-=size);
            TYPE* tmp=(TYPE*)realloc(DataTab, memSize*sizeof(TYPE));
            if(!tmp)
            {   free(DataTab);
                std::cout<<"Memory allocation error!\n";
            }
            else DataTab=tmp;
        }
        else
        {
            if(position==0&&size>=tabSize)
                clear();
            else
                std::cout<<"Position larger than table size!\n";
        }
    }
    void minMaxSortD()
    {
        if(tabSize>0)
        {
            double mn, mx;
            for(int j=0; j<tabSize; j++)
            for(int i=1; i<tabSize; i++)
            {
                mx=(double)DataTab[i];
                mn =(double)DataTab[i-1];
                if(mx<mn)
                {
                    DataTab[i]=(TYPE)mn;
                    DataTab[i-1]=(TYPE)mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void maxMinSortD()
    {
        if(tabSize>0)
        {
            double mn, mx;
            for(int j=0; j<tabSize; j++)
            for(int i=1; i<tabSize; i++)
            {
                mx=(double)DataTab[i];
                mn =(double)DataTab[i-1];
                if(mx>mn)
                {
                    DataTab[i]=(TYPE)mn;
                    DataTab[i-1]=(TYPE)mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void minMaxSortL()
    {
        if(tabSize>0)
        {
            long mn, mx;
            for(int j=0; j<tabSize; j++)
            for(int i=1; i<tabSize; i++)
            {
                mx = DataTab[i];
                mn = DataTab[i-1];
                if(mx<mn)
                {
                    DataTab[i]=mn;
                    DataTab[i-1]=mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void maxMinSortL()
    {
        if(tabSize>0)
        {
            long mn, mx;
            for(int j=0; j<tabSize; j++)
            for(int i=1; i<tabSize; i++)
            {
                mx = DataTab[i];
                mn = DataTab[i-1];
                if(mx>mn)
                {
                    DataTab[i]=mn;
                    DataTab[i-1]=mx;
                }
            }
        }
        else
            std::cout<<"Empty list connot be sorted !\n";
    }
    void print(int i)
    {
        std::cout<<DataTab[i]<<" ";
    }
    void printD(int i)
    {
        //std::cout<<setprecision(precision)<<*(double*)&DataTab[i]<<" ";
        std::cout<<DataTab[i]<<" ";
    }
    void printAll(const char*title="")
    {
        std::cout<<title<<"\n{ ";
        for(int i=0; i<tabSize; i++)
            print(i);
        std::cout<<"}\n";
    }
    void printAllD(const char*title="")
    {
        std::cout<<title<<"\n{ ";
        for(int i=0; i<tabSize; i++)
            printD(i);
        std::cout<<"}\n";
    }
    void operator+=(TYPE data)
    {
        insertEnd(data);
    }
    /*TYPE& operator+=(const TYPE data[], size_t n)
    {
        if(sizeof(data))
            insertTableAt(data, tabSize, sizeof(data)/sizeof(data[0]));
        return *this;
    }*/
    void operator+=(csLIST_DATA<TYPE> data)
    {
        insertAt(data.data,data.position);
    }
    void operator+=(csLIST_DATATAB<TYPE> data)
    {
        insertTableAt(data.data,data.position,data.size);
    }
    void operator+=(csLIST_DATATABPART<TYPE> data)
    {
        insertTablePartAt(data.position,data.data,data.begin,data.size);
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
        if(tabSize)
        {
            free(DataTab);
            DataTab=NULL;
            tabSize=0;
            memSize=0;
        }
    }
};


#endif // CSLIST_H_INCLUDED
