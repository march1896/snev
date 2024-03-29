template <int p, int i>
class is_prime {
public:
   enum { prim = (p==2) || (p%i) && is_prime<(i>2?p:0),i-1>::prim
        }; 
}; 
 
template<>
class is_prime<0,0> {
public:
   enum {prim=1};
}; 
 
template<>
class is_prime<0,1> {
 public:
   enum {prim=1};
}; 
 
template <int i>
class D {
public:
   D(void*);
}; 
 
template <int i>
class Prime_print {      // primary template for loop to print prime numbers
public:
   Prime_print<i-1> a; 
   enum { prim = is_prime<i,i-1>::prim
        }; 
   void f() {
       D<i> d = prim ? 1 : 0;
       a.f();
   } 
}; 
 
template<>
class Prime_print<1> {   // full specialization to end the loop
  public:
    enum {prim=0}; 
    void f() {
        D<1> d = prim ? 1 : 0;
    }; 
}; 
 
#ifndef LAST 
#define LAST 18 
#endif 
 
int main()
{
   Prime_print<LAST> a; 
   a.f(); 
}

