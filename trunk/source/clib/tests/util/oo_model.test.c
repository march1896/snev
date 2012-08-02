#include <stdio.h>
#include <stdlib.h>
#include <oo_model.h>

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


