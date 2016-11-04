#include "smh_common.h"

void smh_stage_add_object(smh_stage *stage, smh_object *obj) {
	obj->next = stage->object;
	stage->object = obj;
}

smh_stage *smh_create_stage(smh_game *game, int type) {
	smh_stage *stage = malloc(sizeof(smh_stage));
	stage->background = 0xFF000000;
	stage->type = type;
	stage->object = NULL;
	stage->jump_count = -1;
	stage->width = 192;
	stage->height = 128;
	if (type == 1) {
		stage->height = 128 * 6;
		struct {
			int type;
			double x, y;
			int width, height;
		} objs[] = {
			{1, 0, 752, 192, 16},
			{1, 0, 688, 28, 16},
			{1, 164, 728, 28, 16},
			{1, 120, 688, 28, 16},
			{1, 64, 638, 28, 16},
			{1, 134, 598, 58, 16},
			{1, 0, 558, 86, 16},
			{1, 0, 542, 70, 16},
			{1, 0, 526, 54, 16},
			{1, 0, 510, 38, 16},
			{1, 0, 494, 22, 16},
			{1, 48, 446, 54, 16},
			{1, 96, 398, 32, 16},
			{1, 128, 350, 64, 16},
			{1, 144, 334, 48, 16},
			{1, 160, 318, 32, 16},
			{1, 176, 302, 16, 16},
			{1, 0, 286, 96, 16},
			{1, 0, 270, 80, 16},
			{1, 0, 254, 64, 16},
			{1, 0, 238, 48, 16},
			{1, 0, 222, 32, 16},
			{1, 0, 206, 16, 16},
			{1, 128, 222, 64, 16},
			{1, 64, 162, 64, 16},
			{1, 64, 114, 16, 48},
			{1, 0, 82, 16, 16},
			{1, 48, 34, 134, 16},
			{2, 92, 740, -1, -1},
			{4, 176, 18, -1, -1},
			{0, -1, -1, -1, -1}
		};
		int i;
		for (i = 0; objs[i].type != 0; i++) {
			smh_object *obj = smh_create_object(game, objs[i].type);
			obj->x = objs[i].x;
			obj->y = objs[i].y;
			if (objs[i].width != -1) {
				obj->width = objs[i].width;
				obj->height = objs[i].height;
			}
			smh_stage_add_object(stage, obj);
			if (obj->focus) stage->character = obj;
		}
	} else if (type == 2) {
		smh_object *obj_message = smh_create_object(game, 3);
		smh_stage_add_object(stage, obj_message);
		smh_object *obj_block = smh_create_object(game, 1);
		obj_block->x = 0; obj_block->y = 112;
		obj_block->width = 192; obj_block->height = 16;
		smh_stage_add_object(stage, obj_block);
	}
	return stage;
}

void smh_destroy_stage(smh_stage *stage) {
	while (stage->object != NULL) {
		smh_object *obj = stage->object->next;
		smh_destroy_object(stage->object);
		free(stage->object);
		stage->object = obj;
	}
	free(stage);
}

smh_object *test_collision(smh_stage *stage, int tx, int ty, int tw, int th) {
	smh_object *obj = stage->object, *ret = NULL;
	int tx1 = tx + tw, ty1 = ty + th;
	while (obj != NULL) {
		if (obj->collision_mode) {
			int x = (int) obj->x, y = (int) obj->y, x1 = x + obj->width, y1 = y + obj->height;
			if (tx <= x1 && x < tx1 && ty <= y1 && y < ty1) {
				ret = obj;
				if (obj->collision_mode == 1) break;
			}
		}
		obj = obj->next;
	}
	return ret;
}

smh_stage *smh_update_stage(smh_stage *stage, smh_game *game, ullong time) {
	if (stage->type == 2) {
		if (stage->jump_count == 1) return smh_create_stage(game, 1);
		if (game->key) stage->jump_count = 1;
		return NULL;
	}
	smh_object *obj = stage->object;
	smh_object *deleteList[256];
	int deleteListTop = 0, i;
	//
	smh_object *chara = stage->character;
	if (chara != NULL) {
		int step = MAX(ABS((int) chara->vx), ABS((int) chara->vy)) + 1;
		double vvx = chara->vx / step, vvy = chara->vy / step;
		double x = chara->x, y = chara->y;
		int w = chara->width, h = chara->height;
		int collidedDirection = 0;
		smh_object *collidedObject = NULL;
		for (i = 1; i <= step; i++) {	// 本来x, yは別々にやるべき(壁への張り付き)
			collidedObject = test_collision(stage, (int) (x + i * vvx), (int) (y + i * vvy), w, h);
			// TODO: アイテム取得処理
			if (collidedObject != NULL) {
				if (collidedObject->type == 4) {
					return smh_create_stage(game, 1);
				}
				if (collidedObject->collision_mode == 1) break;
				collidedObject = NULL;
			}
		}
		i--;
		x = chara->x += vvx * i;
		y = chara->y += vvy * i;
		if (collidedObject != NULL) {
			if (vvx > 0 && test_collision(stage, (int)x + 1, (int)y, w, h) != NULL) chara->vx = 0, collidedDirection |= 1;
			if (vvy > 0 && test_collision(stage, (int)x, (int)y + 1, w, h) != NULL) chara->vy = 0, collidedDirection |= 2;
			if (vvx < 0 && test_collision(stage, (int)x - 1, (int)y, w, h) != NULL) chara->vx = 0, collidedDirection |= 4;
			if (vvy < 0 && test_collision(stage, (int)x, (int)y - 1, w, h) != NULL) chara->vy = 0, collidedDirection |= 8;
		}
		if (collidedDirection & 2) {
			if (stage->jump_count <= 0) stage->jump_count = 4;
		} else chara->vy += 0.3;
		if (game->key & KEY_UP && stage->jump_count-- > 0) chara->vy -= 1.6, chara->vx *= 0.95;
		else stage->jump_count = -1;
		chara->vx *= (collidedDirection & 2) ? 0.9 : 0.95;
		if (game->key & KEY_LEFT) chara->vx -= (collidedDirection & 2) ? 0.4 : 0.05;
		if (game->key & KEY_RIGHT) chara->vx += (collidedDirection & 2) ? 0.4 : 0.05;
		if (chara->vx > 3) chara->vx = 3;
		if (chara->vx < -3) chara->vx = -3;
		if (chara->y > 1000) {
			return smh_create_stage(game, 2);
		}
	}
	//
	while (obj != NULL) {
		if (smh_update_object(obj, game, time)) {
			deleteList[deleteListTop++] = obj;
		}
		obj = obj->next;
	}
	obj = stage->object;
	i = 0;
	while (obj == deleteList[i]) {
		i++;
		stage->object = obj = obj->next;
	}
	while (i < deleteListTop) {
		if (obj->next == deleteList[i]) {
			i++;
			obj->next = obj->next->next;
		} else {
			obj = obj->next;
		}
	}
	for (i = 0; i < deleteListTop; i++) {
		smh_destroy_object(deleteList[i]);
	}
	return NULL;
}

void smh_draw_stage(smh_stage *stage, smh_game *game, smh_graphic *g) {
	smh_draw_rectangle(game->win, g, 0, 0, 192, 128, stage->background);
	smh_object *obj = stage->object;
	int rx = 0, ry = 0;
	if (stage->character != NULL) {
		rx = stage->character->x - 96;
		ry = stage->character->y - 64;
		if (rx < 0) rx = 0;
		if (ry < 0) ry = 0;
		if (rx + 192 > stage->width) rx = stage->width - 192;
		if (ry + 128 > stage->height) ry = stage->height - 128;
	}
	while (obj != NULL) {
		if (obj->x > rx - obj->width && obj->x < rx + 192 &&
				obj->y > ry - obj->height && obj->y < ry + 128) {
			smh_draw_object(obj, game, g, obj->x - (double) rx, obj->y - (double) ry);
		}
		obj = obj->next;
	}
}

