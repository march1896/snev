/*
 * A complete object-orient model in C.
 *
 * suppose we want to realize the below code in c.

 
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

/* Since we have no access right related keywords in C, for clear, the following 
 * comment will be add in front of each declaration and definiation. */
/* public */     /* public, could exposed in *.h */
/* protected */  /* protected, should be visible for derived class, but not exposed */
/* private */    /* private, should be contained in *.c */

#include <stdio.h>
#include <stdlib.h>
/* befor everything */
// erase any type information of class, this is unknown
typedef void* unknown;
// unique_id tells us about the type of unknown type
typedef int unique_id;

/***********************************************************************************************
 * start if animal interface 
 ***********************************************************************************************/
// define of interface animal
/* public */ unique_id animal_int = 0;
/* public */ void eat(unknown obj, int food);  // the first parameter is always pointer to the object.
/* public */ void breath(unknown obj);

// virtual funtion declaration, this should be the same as public interface, exclude pointer to object.
/* protected */ typedef void (*pf_eat)(int food);
/* protected */ typedef void (*pf_breath)();

// virtual table declaration, definition is only needed for non virtual class.
/* protected */ typedef struct {
	pf_eat eat;
	pf_breath breath;
} animal_operations;

// public interface inplementation.
/* private */ void eat(unknown obj, int food) {
	animal_operations* ops = (animal_operations*)obj;

	return (ops->eat)(food);
}

/* private */ void breath(unknown obj) {
	animal_operations* ops = (animal_operations*)obj;

	return (ops->breath)();
}

/***********************************************************************************************
 * start if flyable interface 
 ***********************************************************************************************/
// interface declaration.
/* public */ unique_id flyable_int = 1;
/* public */ void fly(unknown object);

// virtual function declaration.
/* protected */ typedef void (*pf_fly)();
// virtual table declaration.
/* protected */ typedef struct {
	pf_fly fly;
} flyable_operations;

/* private */ void fly(unknown obj) {
	flyable_operations* ops = (flyable_operations*)obj;

	return (ops->fly)();
}

/***********************************************************************************************
 * start if bird class 
 ***********************************************************************************************/
// class definition.
typedef unknown bird;

// class basic method declaration.
/* public */ bird bird_create(); // constructor
/* public */ void bird_destroy(bird); // destructor
// cast, always run-time cast, since compiler will not static cast for us.
/* public */ unknown bird_cast(bird pb, unique_id interface_id); 


// functionality implements 
/* protected */ void bird_local_fly() { printf("bird fly\n"); }
/* protected */ void bird_local_breath() { printf("bird breath\n"); }
/* protected */ void bird_local_eat(int food) { printf("bird eat %d\n", food); }

// vitual table declaration.
/* protected */ typedef struct {
	animal_operations* animal_ops;
	flyable_operations* flyable_ops;
} bird_operations;

// virtual table definition.
/* private */ static animal_operations bird_animal_vtb = {
	bird_local_eat,
	bird_local_breath,
};

/* private */ static flyable_operations bird_flyable_vtb = {
	bird_local_fly
};

/* private */ static bird_operations bird_all_vtb = {
	&bird_animal_vtb,
	&bird_flyable_vtb
};

// concrete type information.
/* protected */ typedef struct bird_local_t {
	int age, info, anything;
	bird_operations* ops;
} bird_local;

// constructor, destructor implementation
/* private */ bird bird_create() {
	bird_local* lb = (bird_local*)malloc(sizeof(bird_local));

	lb->age = lb->info = lb->anything = 0;
	lb->ops = &bird_all_vtb;

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
	return (unknown)NULL;
}

/***********************************************************************************************
 * start if swallow class 
 ***********************************************************************************************/
// class definition.
typedef unknown swallow;

// class basic method declaration.
/* public */ swallow swallow_create(); // constructor
/* public */ void swallow_destroy(swallow); // destructor
// cast, always run-time cast, since compiler will not static cast for us.
/* public */ unknown swallow_cast(swallow pb, unique_id interface_id); 


// functionality implements 
/* protected */ void swallow_local_fly() { printf("swallow fly\n"); }
/* protected */ void swallow_local_eat(int food) { printf("swallow eat %d\n", food); }

// vitual table declaration.
/* protected */ typedef struct {
	animal_operations* animal_ops;
	flyable_operations* flyable_ops;
} swallow_operations;

// virtual table definition.
/* private */ static animal_operations swallow_animal_vtb = {
	swallow_local_eat,
	bird_local_breath, // super breathe
};

/* private */ static flyable_operations swallow_flyable_vtb = {
	swallow_local_fly
};

/* private */ static swallow_operations swallow_all_vtb = {
	&swallow_animal_vtb,
	&swallow_flyable_vtb
};

// concrete type information.
/* protected */ typedef struct swallow_local_t {
	int age, info, anything;
	swallow_operations* ops;
} swallow_local;

// constructor, destructor implementation
/* private */ swallow swallow_create() {
	swallow_local* lb = (swallow_local*)malloc(sizeof(swallow_local));

	lb->age = lb->info = lb->anything = 0;
	lb->ops = &swallow_all_vtb;

	// do initialize work
	return (swallow)lb;
}

/* private */ void swallow_destroy(swallow b) {
	free(b);
}

// cast implementation
/* private */ unknown swallow_cast(swallow pb, unique_id interface_id) {
	swallow_local* lb = (swallow_local*) pb;
	if (interface_id == animal_int) {
		return (unknown)((lb->ops)->animal_ops);
	}
	else if (interface_id == flyable_int) {
		return (unknown)((lb->ops)->flyable_ops);
	}
	return (unknown)NULL;
}

/***********************************************************************************************
 * start if magpie class 
 ***********************************************************************************************/
// class definition.
typedef unknown magpie;

// class basic method declaration.
/* public */ magpie magpie_create(); // constructor
/* public */ void magpie_destroy(magpie); // destructor
// cast, always run-time cast, since compiler will not static cast for us.
/* public */ unknown magpie_cast(magpie pb, unique_id interface_id); 


// functionality implements 
/* protected */ void magpie_local_fly() { printf("magpie fly\n"); }
/* protected */ void magpie_local_eat(int food) { printf("magpie eat %d\n", food); }

// vitual table declaration.
/* protected */ typedef struct {
	animal_operations* animal_ops;
	flyable_operations* flyable_ops;
} magpie_operations;

// virtual table definition.
/* private */ static animal_operations magpie_animal_vtb = {
	magpie_local_eat,
	bird_local_breath, // super breathe
};

/* private */ static flyable_operations magpie_flyable_vtb = {
	magpie_local_fly
};

/* private */ static magpie_operations magpie_all_vtb = {
	&magpie_animal_vtb,
	&magpie_flyable_vtb
};

// concrete type information.
/* protected */ typedef struct magpie_local_t {
	int age, info, anything;
	magpie_operations* ops;
} magpie_local;

// constructor, destructor implementation
/* private */ magpie magpie_create() {
	magpie_local* lb = (magpie_local*)malloc(sizeof(magpie_local));

	lb->age = lb->info = lb->anything = 0;
	lb->ops = &magpie_all_vtb;

	// do initialize work
	return (magpie)lb;
}

/* private */ void magpie_destroy(magpie b) {
	free(b);
}

// cast implementation
/* private */ unknown magpie_cast(magpie pb, unique_id interface_id) {
	magpie_local* lb = (magpie_local*) pb;
	if (interface_id == animal_int) {
		return (unknown)((lb->ops)->animal_ops);
	}
	else if (interface_id == flyable_int) {
		return (unknown)((lb->ops)->flyable_ops);
	}
	return (unknown)NULL;
}


int main() {
	unknown bird = bird_create();
	eat(bird_cast(bird, animal_int), 10);
	fly(bird_cast(bird, flyable_int));
	breath(bird_cast(bird, animal_int));
	bird_destroy(bird);

	printf("\n");

	unknown swallow = swallow_create();
	eat(swallow_cast(swallow, animal_int), 8);
	fly(swallow_cast(swallow, flyable_int));
	breath(swallow_cast(swallow, animal_int));
	swallow_destroy(swallow);

	printf("\n");

	unknown magpie = magpie_create();
	eat(magpie_cast(magpie, animal_int), 8);
	fly(magpie_cast(magpie, flyable_int));
	breath(magpie_cast(magpie, animal_int));
	magpie_destroy(magpie);
}


