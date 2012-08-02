#ifndef _OBJECT_ORIENTED_MODEL_
#define _OBJECT_ORIENTED_MODEL_
/*
 * This is a document file, no *.c file should include this file, But all demo code could be 
 * compiled and run. It demostrates a object-oriented coding pattern in C language.
 *
 * Before designing, for simplicity, we make following assumptions.
 * 1, clients know nothing about classes, they only works with interface, and factories.
 * 2, the model contains run time information, and can be casted.
 * 3, when using interface method, we can not apply it directly on class, we must cast the class 
 *    into proper interface.
 *
 * For polymorphism, interface method did nothing itself but only call the correct callback(virtual
 *    funtion) that is defined in the sub class. To bind the interface function and virtual funtion 
 *    together, they must have the same name, concrete sub-class fuctions could have different name,
 *    and the name is not visible from others, but the callback funtion pointer should be the same name
 *    with interface method.
 * The first and third rules together ensures that when calling interface method, the callback set
 *    (virtual function table) is determined and unique(exactly the callbacks of the casted interface).
 *
 * From implementation aspect of view, when we got a pointer that is casted to an interface, we should 
 *    know the offset of the virtual funtion.
 *
 * A complete object-orient model in C.
 */

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
 * comment will be add in front of each declaration and definiation. 
 */

/* public */     /* public, could exposed in *.h */
/* protected */  /* protected, should be visible for derived class, but not exposed */
/* private */    /* private, should be contained in *.c */

#include <stdio.h>
#include <stdlib.h>
/* befor everything */
// erase any type information of class, this is unknown
typedef void* unknown;
// unique_id decribes run-time(compile) information, since no oo in c, so compile time info in also 
// described by it.
typedef int unique_id;
// since compile will can not cast to father from child(change to the right virtual pointer), we should 
// do it our self.
typedef unknown (*pf_cast)(unknown obj, unique_id interface_id);

typedef struct unknown_structure_t {
	void*     ops;
	unique_id rtti;
	pf_cast   cast;
	/* 
	 * points to the object's address, since when object is casted, we may got an offset to
	 * the original address, but when applying another cast, we should do it on the original 
	 * object, addr helps us to it.
	 */
	unknown   addr;   
} unknown_structure;

unknown cast(unknown obj, unique_id interface_id) {
	unknown_structure* us = (unknown_structure*)obj;
	return us->cast(obj, interface_id);
}
/***********************************************************************************************
 * start if animal interface 
 ***********************************************************************************************/
// define of interface animal
/* public */ #define animal_int 0
/* public */ void eat(unknown obj, int food);  // the first parameter is always pointer to the object.
/* public */ void breath(unknown obj);

// virtual funtion declaration, this should be the same as public interface, exclude pointer to object.
/* protected */ typedef void (*pf_eat)(int food);
/* protected */ typedef void (*pf_breath)();

// virtual table declaration, definition is only needed for non virtual class.
/* protected */ typedef struct {
	void*     ops;
	unique_id rtti;
	pf_cast   cast;

	/* start of virtual function table */
	pf_eat    eat;
	pf_breath breath;
} animal_operations;

// public interface inplementation, call correct virtual function.
/* private */ void eat(unknown obj, int food) {
	animal_operations* ops = (animal_operations*)obj;

	(ops->eat)(food);
}

/* private */ void breath(unknown obj) {
	animal_operations* ops = (animal_operations*)obj;

	(ops->breath)();
}

/* private */ unknown animal_cast(unknown obj, unique_id interface_id) {
	animal_operations* ops = (animal_operations*)obj;
	if (interface_id == ops->rtti) {
		/* cast to it self */
		return obj;
	}
	return NULL;
}	

/***********************************************************************************************
 * start if flyable interface 
 ***********************************************************************************************/
// interface declaration.
/* public */ #define flyable_int 1
/* public */ void fly(unknown object);

// virtual function declaration.
/* protected */ typedef void (*pf_fly)();
// virtual table declaration.
/* protected */ typedef struct {
	void*     ops;
	unique_id rtti;
	pf_cast   cast;

	pf_fly    fly;
} flyable_operations;

/* private */ void fly(unknown obj) {
	flyable_operations* ops = (flyable_operations*)obj;

	(ops->fly)();
}

/* private */ unknown flyable_cast(unknown obj, unique_id interface_id) {
	flyable_operations* ops = (flyable_operations*)obj;
	if (interface_id == ops->rtti) {
		return obj;
	}

	return NULL;
}

/***********************************************************************************************
 * start if bird class 
 ***********************************************************************************************/
// class definition.
typedef unknown bird;

// class basic method declaration.
/* public */ #define bird_int 2
/* public */ bird bird_create(); // constructor
/* public */ void bird_destroy(bird); // destructor
// cast, always run-time cast, since compiler will not static cast for us.
/* public */ unknown bird_cast(unknown pb, unique_id interface_id); 


// functionality implements 
/* protected */ void bird_local_fly() { printf("bird fly\n"); }
/* protected */ void bird_local_breath() { printf("bird breath\n"); }
/* protected */ void bird_local_eat(int food) { printf("bird eat %d\n", food); }

// vitual table declaration.
/* protected */ typedef struct {
	animal_operations*  animal_ops;
	flyable_operations* flyable_ops;
} bird_operations;

// virtual table definition.
/* private */ static animal_operations bird_animal_vtb = {
	&bird_animal_vtb,
	bird_int,
	bird_cast,

	bird_local_eat,
	bird_local_breath,
};

/* private */ static flyable_operations bird_flyable_vtb = {
	(&bird_flyable_vtb) - 1,
	bird_int,
	bird_cast,

	bird_local_fly
};

/* private */ static bird_operations bird_all_vtb = {
	&bird_animal_vtb,
	&bird_flyable_vtb
};

// concrete type information.
/* protected */ typedef struct bird_local_t {
	bird_operations* ops;
	unique_id        rtti;
	pf_cast          cast;

	int age;
	int info;
	int anything;
} bird_local;

// constructor, destructor implementation
/* private */ bird bird_create() {
	bird_local* lb = (bird_local*)malloc(sizeof(bird_local));

	lb->age = lb->info = lb->anything = 0;

	lb->ops = &bird_all_vtb;
	lb->rtti = bird_int;
	lb->cast = bird_cast;

	// do initialize work
	return (bird)lb;
}

/* private */ void bird_destroy(bird b) {
	free(b);
}

// cast implementation
/* private */ unknown bird_cast(bird pb, unique_id interface_id) {
	bird_local* lb = (bird_local*) pb;
	if (interface_id == animal_int) {
		return (unknown)((lb->ops)->animal_ops);
	}
	else if (interface_id == flyable_int) {
		return (unknown)((lb->ops)->flyable_ops);
	}
	else if (interface_id == bird_int) {
		return pb;
	}
	return (unknown)NULL;
}

/***********************************************************************************************
 * start if swallow class 
 ***********************************************************************************************/
// class definition.
typedef unknown swallow;

// class basic method declaration.
/* public */ #define swallow_int 3
/* public */ swallow swallow_create(); // constructor
/* public */ void swallow_destroy(swallow); // destructor
// cast, always run-time cast, since compiler will not static cast for us.
/* public */ unknown swallow_cast(swallow pb, unique_id interface_id); 


// functionality implements 
/* protected */ void swallow_local_fly() { printf("swallow fly\n"); }
/* protected */ void swallow_local_eat(int food) { printf("swallow eat %d\n", food); }

// virtual table definition.
/* private */ static animal_operations swallow_animal_vtb = {
	&swallow_animal_vtb,
	swallow_int,
	swallow_cast,

	swallow_local_eat,
	bird_local_breath, // super breathe
};

/* private */ static flyable_operations swallow_flyable_vtb = {
	(&swallow_flyable_vtb)-1,
	swallow_int,
	swallow_cast,

	swallow_local_fly
};

/* private */ static bird_operations swallow_all_vtb = {
	&swallow_animal_vtb,
	&swallow_flyable_vtb
};

// concrete type information.
/* protected */ typedef struct swallow_local_t {
	bird_operations* ops;
	unique_id        rtti;
	pf_cast          cast;

	bird_local       super;
	int swallow_local_data;
} swallow_local;

// constructor, destructor implementation
/* private */ swallow swallow_create() {
	swallow_local* lb = (swallow_local*)malloc(sizeof(swallow_local));

	/* super's default setting */
	(lb->super).age = (lb->super).info = (lb->super).anything = 0;
	/* super's virtual table should never be visited */
	(lb->super).ops = &swallow_all_vtb;

	lb->ops = &swallow_all_vtb;
	lb->rtti = swallow_int;
	lb->cast = swallow_cast;

	// do initialize work
	return (swallow)lb;
}

/* private */ void swallow_destroy(swallow b) {
	free(b);
}

// cast implementation
/* private */ unknown swallow_cast(swallow pb, unique_id interface_id) {
	swallow_local* lb = (swallow_local*) pb;
	if (interface_id == swallow_int || interface_id == bird_int) {
		return pb;
	}
	return bird_cast(pb, interface_id);
}

/***********************************************************************************************
 * start if magpie class 
 ***********************************************************************************************/
// class definition.
typedef unknown magpie;

// class basic method declaration.
/* public */ #define magpie_int 4
/* public */ magpie magpie_create(); // constructor
/* public */ void magpie_destroy(magpie); // destructor
// cast, always run-time cast, since compiler will not static cast for us.
/* public */ unknown magpie_cast(magpie pb, unique_id interface_id); 


// functionality implements 
/* protected */ void magpie_local_fly() { printf("magpie fly\n"); }
/* protected */ void magpie_local_eat(int food) { printf("magpie eat %d\n", food); }

// virtual table definition.
/* private */ static animal_operations magpie_animal_vtb = {
	&magpie_animal_vtb,
	magpie_int,
	magpie_cast,

	magpie_local_eat,
	bird_local_breath, // super breathe
};

/* private */ static flyable_operations magpie_flyable_vtb = {
	(&magpie_animal_vtb)-1,
	magpie_int,
	magpie_cast,

	magpie_local_fly
};

/* private */ static bird_operations magpie_all_vtb = {
	&magpie_animal_vtb,
	&magpie_flyable_vtb
};

// concrete type information.
/* protected */ typedef struct magpie_local_t {
	bird_operations* ops;
	unique_id        rtti;
	pf_cast          cast;

	bird_local super;
	int magpie_local_data;
} magpie_local;

// constructor, destructor implementation
/* private */ magpie magpie_create() {
	magpie_local* lb = (magpie_local*)malloc(sizeof(magpie_local));

	/* super's default setting */
	(lb->super).age = (lb->super).info = (lb->super).anything = 0;
	(lb->super).ops = &magpie_all_vtb;

	lb->ops = &magpie_all_vtb;
	lb->rtti = magpie_int;
	lb->cast = magpie_cast;


	// do initialize work
	return (magpie)lb;
}

/* private */ void magpie_destroy(magpie b) {
	free(b);
}

// cast implementation
/* private */ unknown magpie_cast(magpie pb, unique_id interface_id) {
	magpie_local* lb = (magpie_local*) pb;
	if (interface_id == magpie_int || interface_id == bird_int) {
		return pb;
	}
	return bird_cast(pb, interface_id);
}

int __run_test() {
	unknown bd, swa, mag;
	unknown unk, casted;
	bd = bird_create();
	eat(cast(bd, animal_int), 10);
	fly(cast(bd, flyable_int));
	breath(cast(bd, animal_int));

	printf("\n");

	swa = swallow_create();
	eat(cast(swa, animal_int), 8);
	fly(cast(swa, flyable_int));
	breath(cast(swa, animal_int));

	printf("\n");

	mag = magpie_create();
	eat(cast(mag, animal_int), 8);
	fly(cast(mag, flyable_int));
	breath(cast(mag, animal_int));

	casted = cast(bd, animal_int);
	unk = cast(casted, flyable_int);

	fly(unk);
}

#endif /* _OBJECT_ORIENTED_MODEL_ */

