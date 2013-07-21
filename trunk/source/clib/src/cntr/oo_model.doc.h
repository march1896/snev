#ifndef _OBJECT_ORIENTED_MODEL_
#define _OBJECT_ORIENTED_MODEL_
/*
 * This is a document file, no *.c file should include this file, but all demo code could be 
 * compiled and run. It demonstrates a object-oriented coding pattern in C language.
 *
 * Before designing, for simplicity, we make following assumptions.
 * 1, clients know nothing about classes, they only works with interface, and factories.
 * 2, the model contains run time information, and can be casted.
 * 3, when using interface method, we can not apply it directly on class, we must cast the class 
 *    into proper interface.
 *
 * For polymorphisms, interface method did nothing itself but only call the correct callback(virtual
 *    function) that is defined in the sub class. To bind the interface function and virtual function 
 *    together, they must have the same name, concrete sub-class functions could have different name,
 *    and the name is not visible from others, but the callback function pointer should be the same name
 *    with interface method.
 * The first and third rules together ensures that when calling interface method, the callback set
 *    (virtual function table) is determined and unique(exactly the callbacks of the casted interface,
 *    there is no virtual pointer combination problem when make interface calls).
 *
 * From implementation aspect of view, when we got a pointer that is casted to an interface, we should 
 *    know the offset of the virtual functions, since we know the type of the interface.
 * A problem is that when casting a sub class to an interface, we should get a pointer to the structure
 *    if the interface's virtual table, the sub class's information is totally missed, then when we want
 *    to cast this interface into another, we should call the right cast function, but since the sub-class
 *    information is missed, we could not find the cast_virtual_function by name.
 * Solution is put __this and __cast in relative fixed position to each interface and class, so we could 
 *    use the relative offset to call the virtual function.
 *
 * A complete object-orient model in C.
 */

/*
 * In fact this model does more than C++'s object model in the following example.
 * class A;
 * class B;
 * class X : public A, B;
 * X* x = new X();
 * A* a = (A*)x; // cast a object to interface, compiler will do some offset job.
 * B* b = (A*)a; // cast from another interface, compiler will only complains
 * but in this model, the problem is resolved, of course increase the complicity.
 */

/* before everything */
// erase any type information of class, this is unknown
typedef void* unknown;
// unique_id describes run-time(compile) information, since no oo in c, so compile time info in also 
// described by it.
typedef int unique_id;
// since compile will can not cast to father from child(change to the right virtual pointer), we should 
// do it our self.
typedef unknown (*pf_cast)(unknown obj, unique_id interface_id);

typedef struct unknown_interface_vtable_structure_t {
	/*
	 * static cast is implemented by compiler in OO language, it just compute the offset of parent body.
	 *   in C, we should provide it ourself, by provide the virtual function, we can always call the right
	 *   function of the concrete class, that class should handle all casting cases.
	 * dynamic cast is also supported by compiler, since it knowns class hierarchy, the compiler should provide
	 *   an implicate cast method if a father class can cast into sub class. For a child class we can easy 
	 *   know which parent class it can cast to, so by virtual call to child class' cast function, we should
	 *   know all kinds of class the current object can cast to.
	 *
	 * The cast function will always points the concrete created class.
	 */
	pf_cast                    __cast;
	/* 
	 * interface specific functions are defined below.
	 */
} unknown_interface_vtable;

typedef struct unknown_interface_body_t {
	/*
	 * for interface method, it should know which concrete function it calls(by virtual table), and 
	 * which data it operates on(by the offset to this pointer. So to perform a virtual function call
	 * we should at least save the virtual function pointer, and this pointer.
	 */
	unknown                   __this;
	unknown_interface_vtable* __vt;
	/* 
	 * interface do not have member variables, for classes members are defined below
	 */
} unknown_interface_body;

unknown __cast(unknown body, unique_id id) {
	/*
	 * since all object's body memory layout follows unknown_interface_body, we can assume that the first 
	 * member is pointer to __this, and the second member is pointer to __vt.
	 */
	unknown_interface_vtable* vt = ((unknown_interface_body*)body)->__vt;

	/*
	 * since all object's virtual table memory layout follows unknown_interface_vtable, we can assume 
	 * that the first member is pointer to __cast.
	 */
	return vt->__cast(((unknown_interface_body*)body)->__this, id);
}

typedef struct unknown_class_vtable_structure_t {
	/*
	 * see definition of unknown_interfact_vtable.
	 */
	pf_cast                   __cast;

	/*
	 * new defined virtual functions
	*/
} unknown_class_vtable;

typedef struct unknown_class_body_t {
	unknown                  __this;
	unknown_class_vtable*    __vt;

	/*
	 * interface body that this class inherit
	 */
	unknown_interface_body   __pib;

	unknown                  class_specific_variables;
} unknown_class_body;

/******************************************************************************************************************
 *
 ******************************************************************************************************************
 *
 ******************************************************************************************************************
 *
 ******************************************************************************************************************
 *                       A concrete example.
 ******************************************************************************************************************
 *
 ******************************************************************************************************************
 *
 ******************************************************************************************************************
 */

#define interface_function_implement_noparam(inter_name, return_type, function_name) \
	return_type function_name(unknown body) {\
		inter_name##_interface_vtable* vt = (inter_name##_interface_vtable*)((unknown_interface_body*)body)->__vt;\
		return vt->__##function_name(((unknown_interface_body*)body)->__this);\
	}

#define interface_function_implement_oneparam(inter_name, return_type, function_name, param_type) \
	return_type function_name(unknown body, param_type param) {\
		inter_name##_interface_vtable* vt = (inter_name##_interface_vtable*)((unknown_interface_body*)body)->__vt;\
		return vt->__##function_name(((unknown_interface_body*)body)->__this, param);\
	}

/***********************************************************************************************/
/*
 suppose we want to realize the below code in c:
 
	interface animal {
		void eat();  // virtual function

		void breath(); // virtual function
	}

	interface flyable {
		void fly();  // virtual function
	}

	class bird implements animal, flyable {
		void eat() { print "bird eat!" }

		void breath() { print "bird breath!" }

		void fly() { print "bird fly!" }
	}

	class swallow extends bird {
		void eat() { print "swallow eat!" }

		void breath() { super.breath() }

		void fly() { print "swallow fly!" }
	}

	class magpie extends bird {
		void eat() { print "magpie eat!" }

		void breath() { super.breath() }

		void fly() { print "magpie eat!" }
	}
*/

/* 
 * Since we have no access right related keywords in C, for clear, the following 
 * comment will be add in front of each declaration and definition. 
 */

#define __public 
#define __protected
#define __private

#include <stdio.h>
#include <stdlib.h>
/***********************************************************************************************
 * start if animal interface 
 ***********************************************************************************************/
__public unique_id animal_int = 0;

// public interface animal
__public void eat(unknown obj);
__public void set_eat(unknown obj, int how_many);
__public void breath(unknown obj);

/* virtual function table */
__protected typedef void (*pf_eat)(unknown body);
__protected typedef void (*pf_set_eat)(unknown body, int food);
__protected typedef void (*pf_breath)(unknown body);

__protected typedef struct {
	pf_cast                   __cast;
	/* start of virtual function table */
	pf_eat                    __eat;
	pf_set_eat                __set_eat;
	pf_breath                 __breath;
} animal_interface_vtable;

interface_function_implement_noparam(animal, void, eat);
interface_function_implement_oneparam(animal, void, set_eat, int);
interface_function_implement_noparam(animal, void, breath);

/*
 * interface body are the same, we do not need to define a new body here.
 */

/***********************************************************************************************
 * start if flyable interface 
 ***********************************************************************************************/
__public int flyable_int = 1;

// interface declaration.
__public void fly(unknown);

__protected typedef void (*pf_fly)(unknown);
__protected typedef struct {
	pf_cast                   __cast;
	/* start of virtual function table */
	pf_fly                    __fly;
} flyable_interface_vtable;

interface_function_implement_noparam(flyable, void, fly);

/***********************************************************************************************
 * start if bird class 
 ***********************************************************************************************/
// class definition.
typedef unknown bird;

// class basic method declaration.
__public int bird_int = 2;

// functionality implements 
// virtual table declaration.
__protected typedef struct {
	pf_cast                   __cast;
} bird_class_vtable;

void bird_local_fly(unknown body);
void bird_local_breath(unknown body);
void bird_local_eat(unknown body);
void bird_local_set_eat(unknown body, int food);
unknown bird_cast(unknown body, unique_id id);

__private static animal_interface_vtable bird_animal_vtb = {
	bird_cast,               /* __cast */

	bird_local_eat,          /* __eat */
	bird_local_set_eat,      /* __set_eat */
	bird_local_breath        /* __breath */
};

__private static flyable_interface_vtable bird_flyable_vtb = {
	bird_cast,

	bird_local_fly
};

__private static bird_class_vtable bird_vtb = {
	bird_cast
};

// concrete type information.
__protected typedef struct bird_local_t {
	unknown                  __this;
	bird_class_vtable*       __vt;

	unknown_interface_body   __pbd_animal;
	unknown_interface_body   __pbd_flyable;

	int                      age;
	int                      anything;
} bird_body;

__protected void bird_local_fly(unknown body)    { printf("bird fly\n"); }
__protected void bird_local_breath(unknown body) { printf("bird breath\n"); }
__protected void bird_local_eat(unknown body)    { 
	bird_body* pb = (bird_body*)body;
	printf("bird eat %d\n", pb->anything); 
}
__protected void bird_local_set_eat(unknown body, int food) { 
	bird_body* pb = (bird_body*)body;
	pb->anything = food;
	printf("set bird eat %d\n", pb->anything);
}


// constructor, destructor implementation
__private bird bird_create() {
	bird_body* bb = (bird_body*)malloc(sizeof(bird_body));
	
	bb->__this = (unknown)bb;
	bb->__vt   = &bird_vtb;

	(bb->__pbd_animal).__this = bb->__this;
	(bb->__pbd_animal).__vt = (unknown_interface_vtable*)&bird_animal_vtb;

	(bb->__pbd_flyable).__this = bb->__this;
	(bb->__pbd_flyable).__vt = (unknown_interface_vtable*)&bird_flyable_vtb;

	bb->age = 0;
	bb->anything = 0;

	return (bird)bb;
}

__private void bird_destroy(bird b) {
	free(b);
}

// cast implementation
__private unknown bird_cast(bird pb, unique_id interface_id) {
	bird_body* bb = (bird_body*) pb;
	if (interface_id == animal_int) {
		return (unknown)&(bb->__pbd_animal);
	}
	else if (interface_id == flyable_int) {
		return (unknown)&(bb->__pbd_flyable);
	}
	else if (interface_id == bird_int) {
		return bb;
	}
	return (unknown)NULL;
}

/***********************************************************************************************
 * start if swallow class 
 ***********************************************************************************************/
// class definition.
typedef unknown swallow;

// class basic method declaration.
__public int swallow_int = 3;

__protected typedef struct {
	pf_cast                   __cast;
} swallow_class_vtable;

void swallow_local_fly(unknown body);
void swallow_local_breath(unknown body);
void swallow_local_eat(unknown body);
void swallow_local_set_eat(unknown body, int food);
unknown swallow_cast(unknown body, unique_id id);

__private static animal_interface_vtable swallow_animal_vtb = {
	swallow_cast,               /* __cast */

	swallow_local_eat,          /* __eat */
	swallow_local_set_eat,      /* __set_eat */
	swallow_local_breath        /* __breath */
};

__private static flyable_interface_vtable swallow_flyable_vtb = {
	swallow_cast,

	swallow_local_fly
};

__private static bird_class_vtable swallow_bird_vtb = {
	swallow_cast,
};

__private static swallow_class_vtable swallow_vtb = {
	swallow_cast
};

// concrete type information.
__protected typedef struct swallow_local_t {
	unknown                  __this;
	swallow_class_vtable*     __vt;

	bird_body                __pbd_bird;

	int                      anything;
} swallow_body;

__protected void swallow_local_fly(unknown body)    { printf("swallow fly\n"); }
__protected void swallow_local_breath(unknown body) { printf("swallow breath\n"); }
/*
 * directly inherit parent virtual function need a little work since compile will not find 
 * the correct parent members.
 */
__protected void swallow_local_eat(unknown body)    { 
	swallow_body* pb = (swallow_body*)body;

	/*
	 * when sub class want to directly use parent's virtual function, it can not 
	 * directly set parent's local function in its own virtual table, because when parent
	 * virtual is called, it take parameter __this as it self, so here we should add an 
	 * wrapper on it.
	 */
	bird_body* pbird = &(pb->__pbd_bird);
	bird_local_eat(pbird);
}
__protected void swallow_local_set_eat(unknown body, int food) { 
	swallow_body* pb = (swallow_body*)body;

	bird_body* pbird = &(pb->__pbd_bird);
	bird_local_set_eat(pbird, food);
}


// constructor, destructor implementation
__private swallow swallow_create() {
	swallow_body* bb = (swallow_body*)malloc(sizeof(swallow_body));
	
	bb->__this = (unknown)bb;
	bb->__vt   = &swallow_vtb;


	{
		(bb->__pbd_bird).__this = (unknown)bb;
		(bb->__pbd_bird).__vt   = &swallow_bird_vtb;

		((bb->__pbd_bird).__pbd_animal).__this = bb->__this;
		((bb->__pbd_bird).__pbd_animal).__vt = (unknown_interface_vtable*)&swallow_animal_vtb;

		((bb->__pbd_bird).__pbd_flyable).__this = bb->__this;
		((bb->__pbd_bird).__pbd_flyable).__vt = (unknown_interface_vtable*)&swallow_flyable_vtb;

		(bb->__pbd_bird).age = 0;
		(bb->__pbd_bird).anything = 0;
	}

	bb->anything = 0;

	return (swallow)bb;
}

__private void swallow_destroy(swallow b) {
	free(b);
}

// cast implementation
__private unknown swallow_cast(swallow pb, unique_id interface_id) {
	swallow_body* bb = (swallow_body*) pb;
	if (interface_id == swallow_int) {
		return bb;
	}
	if (interface_id == bird_int) {
		return (unknown)(&(bb->__pbd_bird));
	}
	else {
		return bird_cast(&(bb->__pbd_bird), interface_id);
	}
}
/***********************************************************************************************
 * start if magpie class 
 ***********************************************************************************************/
// class definition.
typedef unknown magpie;

// class basic method declaration.
__public int magpie_int = 4;

__protected typedef struct {
	pf_cast                   __cast;
} magpie_class_vtable;

void magpie_local_fly(unknown body);
void magpie_local_breath(unknown body);
void magpie_local_eat(unknown body);
void magpie_local_set_eat(unknown body, int food);
unknown magpie_cast(unknown body, unique_id id);

__private static animal_interface_vtable magpie_animal_vtb = {
	magpie_cast,               /* __cast */

	magpie_local_eat,          /* __eat */
	magpie_local_set_eat,      /* __set_eat */
	magpie_local_breath        /* __breath */
};

__private static flyable_interface_vtable magpie_flyable_vtb = {
	magpie_cast,

	magpie_local_fly
};

__private static bird_class_vtable magpie_bird_vtb = {
	magpie_cast,
};


__private static magpie_class_vtable magpie_vtb = {
	magpie_cast
};

// concrete type information.
__protected typedef struct magpie_local_t {
	unknown                  __this;
	magpie_class_vtable*     __vt;

	bird_body                __pbd_bird;

	int                      anything;
} magpie_body;

__protected void magpie_local_fly(unknown body)    { printf("magpie fly\n"); }
__protected void magpie_local_breath(unknown body) { printf("magpie breath\n"); }
__protected void magpie_local_eat(unknown body)    { 
	magpie_body* pb = (magpie_body*)body;
	printf("magpie eat %d\n", pb->anything); 
}
__protected void magpie_local_set_eat(unknown body, int food) { 
	magpie_body* pb = (magpie_body*)body;
	pb->anything = food;
	printf("set magpie eat %d\n", pb->anything);
}


// constructor, destructor implementation
__private magpie magpie_create() {
	magpie_body* bb = (magpie_body*)malloc(sizeof(magpie_body));
	
	bb->__this = (unknown)bb;
	bb->__vt   = &magpie_vtb;


	{
		(bb->__pbd_bird).__this = (unknown)bb;
		(bb->__pbd_bird).__vt   = &magpie_bird_vtb;

		((bb->__pbd_bird).__pbd_animal).__this = bb->__this;
		((bb->__pbd_bird).__pbd_animal).__vt = (unknown_interface_vtable*)&magpie_animal_vtb;

		((bb->__pbd_bird).__pbd_flyable).__this = bb->__this;
		((bb->__pbd_bird).__pbd_flyable).__vt = (unknown_interface_vtable*)&magpie_flyable_vtb;

		(bb->__pbd_bird).age = 0;
		(bb->__pbd_bird).anything = 0;
	}

	bb->anything = 0;

	return (magpie)bb;
}

__private void magpie_destroy(magpie b) {
	free(b);
}

// cast implementation
__private unknown magpie_cast(magpie pb, unique_id interface_id) {
	magpie_body* bb = (magpie_body*) pb;
	if (interface_id == magpie_int) {
		return bb;
	}
	if (interface_id == bird_int) {
		return (unknown)(&(bb->__pbd_bird));
	}
	else {
		return bird_cast(&(bb->__pbd_bird), interface_id);
	}
}


int __run_test() {
	unknown bd, swa, mag;
	unknown unk, casted;
	bd = bird_create();
	set_eat(__cast(bd, animal_int), 5);
	eat(__cast(bd, animal_int));
	fly(__cast(bd, flyable_int));
	breath(__cast(bd, animal_int));

	printf("\n");

	swa = swallow_create();
	set_eat(__cast(swa, animal_int), 8);
	eat(__cast(swa, animal_int));
	fly(__cast(swa, flyable_int));
	breath(__cast(swa, animal_int));

	printf("\n");

	mag = magpie_create();
	set_eat(__cast(mag, animal_int), 15);
	eat(__cast(mag, animal_int));
	fly(__cast(mag, flyable_int));
	breath(__cast(mag, animal_int));

	printf("\n");

	casted = __cast(bd, animal_int);
	unk = __cast(casted, flyable_int);
	fly(unk);

	casted = __cast(mag, animal_int);
	unk = __cast(casted, flyable_int);
	fly(unk);

	casted = __cast(swa, animal_int);
	unk = __cast(casted, flyable_int);
	fly(unk);

	casted = __cast(swa, magpie_int);
	printf("swallow cast to magpie : %s\n", casted == NULL ? "false" : "true");

	casted = __cast(mag, magpie_int);
	printf("magpie cast to magpie : %s\n", casted == NULL ? "false" : "true");

	casted = __cast(mag, bird_int);
	printf("magpie cast to bird : %s\n", casted == NULL ? "false" : "true");

	casted = __cast(bd, magpie_int);
	printf("bird cast to magpie : %s\n", casted == NULL ? "false" : "true");
	return 0;
}

#endif /* _OBJECT_ORIENTED_MODEL_ */
