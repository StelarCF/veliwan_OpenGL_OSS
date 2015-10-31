#include "GameObject.h"

void GameObject::update() {
	rendertask->ro->rot = rotation;
	rendertask->ro->pos = position;
	rendertask->ro->recompute();
}