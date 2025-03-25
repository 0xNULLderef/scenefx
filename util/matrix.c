#include <string.h>
#include <wayland-server-protocol.h>
#include <wlr/util/box.h>
#include "util/matrix.h"

void scenefx_matrix_identity(float mat[static 9]) {
	static const float identity[9] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};
	memcpy(mat, identity, sizeof(identity));
}

void scenefx_matrix_multiply(float mat[static 9], const float a[static 9],
		const float b[static 9]) {
	float product[9];

	product[0] = a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
	product[1] = a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
	product[2] = a[0]*b[2] + a[1]*b[5] + a[2]*b[8];

	product[3] = a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
	product[4] = a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
	product[5] = a[3]*b[2] + a[4]*b[5] + a[5]*b[8];

	product[6] = a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
	product[7] = a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
	product[8] = a[6]*b[2] + a[7]*b[5] + a[8]*b[8];

	memcpy(mat, product, sizeof(product));
}

void scenefx_matrix_translate(float mat[static 9], float x, float y) {
	float translate[9] = {
		1.0f, 0.0f, x,
		0.0f, 1.0f, y,
		0.0f, 0.0f, 1.0f,
	};
	scenefx_matrix_multiply(mat, mat, translate);
}

void scenefx_matrix_scale(float mat[static 9], float x, float y) {
	float scale[9] = {
		x,    0.0f, 0.0f,
		0.0f, y,    0.0f,
		0.0f, 0.0f, 1.0f,
	};
	scenefx_matrix_multiply(mat, mat, scale);
}

static const float transforms[][9] = {
	[WL_OUTPUT_TRANSFORM_NORMAL] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
	[WL_OUTPUT_TRANSFORM_90] = {
		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
	[WL_OUTPUT_TRANSFORM_180] = {
		-1.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
	[WL_OUTPUT_TRANSFORM_270] = {
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
	[WL_OUTPUT_TRANSFORM_FLIPPED] = {
		-1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
	[WL_OUTPUT_TRANSFORM_FLIPPED_90] = {
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
	[WL_OUTPUT_TRANSFORM_FLIPPED_180] = {
		1.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
	[WL_OUTPUT_TRANSFORM_FLIPPED_270] = {
		0.0f, -1.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	},
};

void scenefx_matrix_transform(float mat[static 9],
		enum wl_output_transform transform) {
	scenefx_matrix_multiply(mat, mat, transforms[transform]);
}

void scenefx_matrix_project_box(float mat[static 9], const struct wlr_box *box,
		enum wl_output_transform transform, const float projection[static 9]) {
	int x = box->x;
	int y = box->y;
	int width = box->width;
	int height = box->height;

	scenefx_matrix_identity(mat);
	scenefx_matrix_translate(mat, x, y);

	scenefx_matrix_scale(mat, width, height);

	if (transform != WL_OUTPUT_TRANSFORM_NORMAL) {
		scenefx_matrix_translate(mat, 0.5, 0.5);
		scenefx_matrix_transform(mat, transform);
		scenefx_matrix_translate(mat, -0.5, -0.5);
	}

	scenefx_matrix_multiply(mat, projection, mat);
}
