/************************************************************

   cvmat.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2008 Masakazu Yonekura

************************************************************/
#include "cvmat.h"
/*
 * Document-class: OpenCV::CvMat
 *
 * CvMat is basic 2D matrix class in OpenCV.
 *
 * C structure is here.
 *  typedef struct CvMat {
 *    int type;
 *    int step;
 *    int *refcount;
 *    union
 *    {
 *      uchar  *ptr;
 *      short  *s;
 *      int    *i;
 *      float  *fl;
 *      double *db;
 *    } data;
 *  #ifdef __cplusplus
 *    union
 *    {
 *      int rows;
 *      int height;
 *    };
 *    union
 *    {
 *      int cols;
 *      int width;
 *    };
 *  #else
 *    int rows; // number of row
 *    int cols; // number of columns
 *  } CvMat;
 */
__NAMESPACE_BEGIN_OPENCV
__NAMESPACE_BEGIN_CVMAT

#define DRAWING_OPTION(opt) rb_get_option_table(rb_klass, "DRAWING_OPTION", opt)
#define DO_COLOR(opt) VALUE_TO_CVSCALAR(LOOKUP_CVMETHOD(opt, "color"))
#define DO_THICKNESS(opt) NUM2INT(LOOKUP_CVMETHOD(opt, "thickness"))
#define DO_LINE_TYPE(opt) rb_drawing_option_line_type(opt)
#define DO_SHIFT(opt) NUM2INT(LOOKUP_CVMETHOD(opt, "shift"))
#define DO_IS_CLOSED(opt) TRUE_OR_FALSE(LOOKUP_CVMETHOD(opt, "is_closed"))

#define GOOD_FEATURES_TO_TRACK_OPTION(opt) rb_get_option_table(rb_klass, "GOOD_FEATURES_TO_TRACK_OPTION", opt)
#define GF_MAX(opt) NUM2INT(LOOKUP_CVMETHOD(opt, "max"))
#define GF_MASK(opt) MASK(LOOKUP_CVMETHOD(opt, "mask"))
#define GF_BLOCK_SIZE(opt) NUM2INT(LOOKUP_CVMETHOD(opt, "block_size"))
#define GF_USE_HARRIS(opt) TRUE_OR_FALSE(LOOKUP_CVMETHOD(opt, "use_harris"))
#define GF_K(opt) NUM2DBL(LOOKUP_CVMETHOD(opt, "k"))

#define FLOOD_FILL_OPTION(opt) rb_get_option_table(rb_klass, "FLOOD_FILL_OPTION", opt)
#define FF_CONNECTIVITY(opt) NUM2INT(LOOKUP_CVMETHOD(opt, "connectivity"))
#define FF_FIXED_RANGE(opt) TRUE_OR_FALSE(LOOKUP_CVMETHOD(opt, "fixed_range"))
#define FF_MASK_ONLY(opt) TRUE_OR_FALSE(LOOKUP_CVMETHOD(opt, "mask_only"))

#define FIND_CONTOURS_OPTION(opt) rb_get_option_table(rb_klass, "FIND_CONTOURS_OPTION", opt)
#define FC_MODE(opt) NUM2INT(LOOKUP_CVMETHOD(opt, "mode"))
#define FC_METHOD(opt) NUM2INT(LOOKUP_CVMETHOD(opt, "method"))
#define FC_OFFSET(opt) VALUE_TO_CVPOINT(LOOKUP_CVMETHOD(opt, "offset"))

#define OPTICAL_FLOW_HS_OPTION(opt) rb_get_option_table(rb_klass, "OPTICAL_FLOW_HS_OPTION", opt)
#define HS_LAMBDA(opt) NUM2DBL(LOOKUP_CVMETHOD(opt, "lambda"))
#define HS_CRITERIA(opt) VALUE_TO_CVTERMCRITERIA(LOOKUP_CVMETHOD(opt, "criteria"))

#define OPTICAL_FLOW_BM_OPTION(opt) rb_get_option_table(rb_klass, "OPTICAL_FLOW_BM_OPTION", opt)
#define BM_BLOCK_SIZE(opt) VALUE_TO_CVSIZE(LOOKUP_CVMETHOD(opt, "block_size"))
#define BM_SHIFT_SIZE(opt) VALUE_TO_CVSIZE(LOOKUP_CVMETHOD(opt, "shift_size"))
#define BM_MAX_RANGE(opt) VALUE_TO_CVSIZE(LOOKUP_CVMETHOD(opt, "max_range"))

#define FIND_FUNDAMENTAL_MAT_OPTION(opt) rb_get_option_table(rb_klass, "FIND_FUNDAMENTAL_MAT_OPTION", opt)
#define FFM_WITH_STATUS(opt) TRUE_OR_FALSE(LOOKUP_CVMETHOD(opt, "with_status"))
#define FFM_MAXIMUM_DISTANCE(opt) NUM2DBL(LOOKUP_CVMETHOD(opt, "maximum_distance"))
#define FFM_DESIRABLE_LEVEL(opt) NUM2DBL(LOOKUP_CVMETHOD(opt, "desirable_level"))

VALUE rb_klass;

int
rb_drawing_option_line_type(VALUE drawing_option)
{
  VALUE line_type = LOOKUP_CVMETHOD(drawing_option, "line_type");
  if (FIXNUM_P(line_type)) {
    return FIX2INT(line_type);
  }
  else if (line_type == ID2SYM(rb_intern("aa"))) {
    return CV_AA;
  }
  return 0;
}

VALUE
rb_class()
{
  return rb_klass;
}

void define_ruby_class()
{
  if (rb_klass)
    return;
  /*
   * opencv = rb_define_module("OpenCV");
   *
   * note: this comment is used by rdoc.
   */
  VALUE opencv = rb_module_opencv();

  rb_klass = rb_define_class_under(opencv, "CvMat", rb_cObject);
  rb_define_alloc_func(rb_klass, rb_allocate);

  VALUE drawing_option = rb_hash_new();
  rb_define_const(rb_klass, "DRAWING_OPTION", drawing_option);
  rb_hash_aset(drawing_option, ID2SYM(rb_intern("color")), cCvScalar::new_object(cvScalarAll(0)));
  rb_hash_aset(drawing_option, ID2SYM(rb_intern("thickness")), INT2FIX(1));
  rb_hash_aset(drawing_option, ID2SYM(rb_intern("line_type")), INT2FIX(8));
  rb_hash_aset(drawing_option, ID2SYM(rb_intern("shift")), INT2FIX(0));

  VALUE good_features_to_track_option = rb_hash_new();
  rb_define_const(rb_klass, "GOOD_FEATURES_TO_TRACK_OPTION", good_features_to_track_option);
  rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("max")), INT2FIX(0xFF));
  rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("mask")), Qnil);
  rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("block_size")), INT2FIX(3));
  rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("use_harris")), Qfalse);
  rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("k")), rb_float_new(0.04));

  VALUE flood_fill_option = rb_hash_new();
  rb_define_const(rb_klass, "FLOOD_FILL_OPTION", flood_fill_option);
  rb_hash_aset(flood_fill_option, ID2SYM(rb_intern("connectivity")), INT2FIX(4));
  rb_hash_aset(flood_fill_option, ID2SYM(rb_intern("fixed_range")), Qfalse);
  rb_hash_aset(flood_fill_option, ID2SYM(rb_intern("mask_only")), Qfalse);

  VALUE find_contours_option = rb_hash_new();
  rb_define_const(rb_klass, "FIND_CONTOURS_OPTION", find_contours_option);
  rb_hash_aset(find_contours_option, ID2SYM(rb_intern("mode")), INT2FIX(CV_RETR_LIST));
  rb_hash_aset(find_contours_option, ID2SYM(rb_intern("method")), INT2FIX(CV_CHAIN_APPROX_SIMPLE));
  rb_hash_aset(find_contours_option, ID2SYM(rb_intern("offset")), cCvPoint::new_object(cvPoint(0,0)));

  VALUE optical_flow_hs_option = rb_hash_new();
  rb_define_const(rb_klass, "OPTICAL_FLOW_HS_OPTION", optical_flow_hs_option);
  rb_hash_aset(optical_flow_hs_option, ID2SYM(rb_intern("lambda")), rb_float_new(0.0005));
  rb_hash_aset(optical_flow_hs_option, ID2SYM(rb_intern("criteria")), cCvTermCriteria::new_object(cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1, 0.001)));

  VALUE optical_flow_bm_option = rb_hash_new();
  rb_define_const(rb_klass, "OPTICAL_FLOW_BM_OPTION", optical_flow_bm_option);
  rb_hash_aset(optical_flow_bm_option, ID2SYM(rb_intern("block_size")), cCvSize::new_object(cvSize(4, 4)));
  rb_hash_aset(optical_flow_bm_option, ID2SYM(rb_intern("shift_size")), cCvSize::new_object(cvSize(1, 1)));
  rb_hash_aset(optical_flow_bm_option, ID2SYM(rb_intern("max_range")),  cCvSize::new_object(cvSize(4, 4)));

  VALUE find_fundamental_matrix_option = rb_hash_new();
  rb_define_const(rb_klass, "FIND_FUNDAMENTAL_MAT_OPTION", find_fundamental_matrix_option);
  rb_hash_aset(find_fundamental_matrix_option, ID2SYM(rb_intern("with_status")), Qfalse);
  rb_hash_aset(find_fundamental_matrix_option, ID2SYM(rb_intern("maximum_distance")), rb_float_new(1.0));
  rb_hash_aset(find_fundamental_matrix_option, ID2SYM(rb_intern("desirable_level")), rb_float_new(0.99));

  rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
  rb_define_singleton_method(rb_klass, "load", RUBY_METHOD_FUNC(rb_load_imageM), -1);
  // Ruby/OpenCV original functions
  rb_define_method(rb_klass, "method_missing", RUBY_METHOD_FUNC(rb_method_missing), -1);
  rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
  rb_define_method(rb_klass, "inside?", RUBY_METHOD_FUNC(rb_inside_q), 1);
  rb_define_method(rb_klass, "to_IplConvKernel", RUBY_METHOD_FUNC(rb_to_IplConvKernel), 1);
  rb_define_method(rb_klass, "create_mask", RUBY_METHOD_FUNC(rb_create_mask), 0);

  rb_define_method(rb_klass, "width", RUBY_METHOD_FUNC(rb_width), 0);
  rb_define_alias(rb_klass, "columns", "width");
  rb_define_alias(rb_klass, "cols", "width");
  rb_define_method(rb_klass, "height", RUBY_METHOD_FUNC(rb_height), 0);
  rb_define_alias(rb_klass, "rows", "height");
  rb_define_method(rb_klass, "depth", RUBY_METHOD_FUNC(rb_depth), 0);
  rb_define_method(rb_klass, "channel", RUBY_METHOD_FUNC(rb_channel), 0);
  rb_define_method(rb_klass, "data", RUBY_METHOD_FUNC(rb_data), 0);

  rb_define_method(rb_klass, "clone", RUBY_METHOD_FUNC(rb_clone), 0);
  rb_define_method(rb_klass, "copy", RUBY_METHOD_FUNC(rb_copy), -1);
  rb_define_method(rb_klass, "to_8u", RUBY_METHOD_FUNC(rb_to_8u), 0);
  rb_define_method(rb_klass, "to_8s", RUBY_METHOD_FUNC(rb_to_8s), 0);
  rb_define_method(rb_klass, "to_16u", RUBY_METHOD_FUNC(rb_to_16u), 0);
  rb_define_method(rb_klass, "to_16s", RUBY_METHOD_FUNC(rb_to_16s), 0);
  rb_define_method(rb_klass, "to_32s", RUBY_METHOD_FUNC(rb_to_32s), 0);
  rb_define_method(rb_klass, "to_32f", RUBY_METHOD_FUNC(rb_to_32f), 0);
  rb_define_method(rb_klass, "to_64f", RUBY_METHOD_FUNC(rb_to_64f), 0);
  rb_define_method(rb_klass, "vector?", RUBY_METHOD_FUNC(rb_vector_q), 0);
  rb_define_method(rb_klass, "square?", RUBY_METHOD_FUNC(rb_square_q), 0);

  rb_define_method(rb_klass, "to_CvMat", RUBY_METHOD_FUNC(rb_to_CvMat), 0);
  rb_define_method(rb_klass, "sub_rect", RUBY_METHOD_FUNC(rb_sub_rect), -2);
  rb_define_alias(rb_klass, "subrect", "sub_rect");
  rb_define_method(rb_klass, "get_rows", RUBY_METHOD_FUNC(rb_get_rows), -2);
  rb_define_method(rb_klass, "get_cols", RUBY_METHOD_FUNC(rb_get_cols), -2);
  rb_define_method(rb_klass, "each_row", RUBY_METHOD_FUNC(rb_each_row), 0);
  rb_define_method(rb_klass, "each_col", RUBY_METHOD_FUNC(rb_each_col), 0);
  rb_define_alias(rb_klass, "each_column", "each_col");
  rb_define_method(rb_klass, "diag", RUBY_METHOD_FUNC(rb_diag), -1);
  rb_define_alias(rb_klass, "diagonal", "diag");
  rb_define_method(rb_klass, "size", RUBY_METHOD_FUNC(rb_size), 0);
  rb_define_method(rb_klass, "dims", RUBY_METHOD_FUNC(rb_dims), 0);
  rb_define_method(rb_klass, "dim_size", RUBY_METHOD_FUNC(rb_dim_size), 1);
  rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), -2);
  rb_define_alias(rb_klass, "at", "[]");
  rb_define_method(rb_klass, "[]=", RUBY_METHOD_FUNC(rb_aset), -2);
  rb_define_method(rb_klass, "set_data", RUBY_METHOD_FUNC(rb_set_data), 1);
  rb_define_method(rb_klass, "fill", RUBY_METHOD_FUNC(rb_fill), -1);
  rb_define_alias(rb_klass, "set", "fill");
  rb_define_method(rb_klass, "fill!", RUBY_METHOD_FUNC(rb_fill_bang), -1);
  rb_define_alias(rb_klass, "set!", "fill!");
  rb_define_method(rb_klass, "clear", RUBY_METHOD_FUNC(rb_clear), 0);
  rb_define_alias(rb_klass, "set_zero", "clear");
  rb_define_alias(rb_klass, "zero", "clear");
  rb_define_method(rb_klass, "clear!", RUBY_METHOD_FUNC(rb_clear_bang), 0);
  rb_define_alias(rb_klass, "set_zero!", "clear!");
  rb_define_alias(rb_klass, "zero!", "clear!");
  rb_define_method(rb_klass, "identity", RUBY_METHOD_FUNC(rb_set_identity), -1);
  rb_define_method(rb_klass, "identity!", RUBY_METHOD_FUNC(rb_set_identity_bang), -1);
  rb_define_method(rb_klass, "range", RUBY_METHOD_FUNC(rb_range), 2);
  rb_define_method(rb_klass, "range!", RUBY_METHOD_FUNC(rb_range_bang), 2);

  rb_define_method(rb_klass, "reshape", RUBY_METHOD_FUNC(rb_reshape), 1);
  rb_define_method(rb_klass, "repeat", RUBY_METHOD_FUNC(rb_repeat), 1);
  rb_define_method(rb_klass, "flip", RUBY_METHOD_FUNC(rb_flip), -1);
  rb_define_method(rb_klass, "flip!", RUBY_METHOD_FUNC(rb_flip_bang), -1);
  rb_define_method(rb_klass, "split", RUBY_METHOD_FUNC(rb_split), 0);
  rb_define_singleton_method(rb_klass, "merge", RUBY_METHOD_FUNC(rb_merge), -2);
  rb_define_method(rb_klass, "rand_shuffle", RUBY_METHOD_FUNC(rb_rand_shuffle), -1);
  rb_define_method(rb_klass, "rand_shuffle!", RUBY_METHOD_FUNC(rb_rand_shuffle_bang), -1);
  rb_define_method(rb_klass, "lut", RUBY_METHOD_FUNC(rb_lut), 1);
  rb_define_method(rb_klass, "convert_scale", RUBY_METHOD_FUNC(rb_convert_scale), 1);
  rb_define_method(rb_klass, "convert_scale_abs", RUBY_METHOD_FUNC(rb_convert_scale_abs), 1);
  rb_define_method(rb_klass, "add", RUBY_METHOD_FUNC(rb_add), -1);
  rb_define_alias(rb_klass, "+", "add");
  rb_define_method(rb_klass, "sub", RUBY_METHOD_FUNC(rb_sub), -1);
  rb_define_alias(rb_klass, "-", "sub");
  rb_define_method(rb_klass, "mul", RUBY_METHOD_FUNC(rb_mul), -1);
  rb_define_method(rb_klass, "mat_mul", RUBY_METHOD_FUNC(rb_mat_mul), -1);
  rb_define_alias(rb_klass, "*", "mat_mul");
  rb_define_method(rb_klass, "div", RUBY_METHOD_FUNC(rb_div), -1);
  rb_define_alias(rb_klass, "/", "div");
  rb_define_singleton_method(rb_klass, "add_weighted", RUBY_METHOD_FUNC(rb_add_weighted), 5);
  rb_define_method(rb_klass, "and", RUBY_METHOD_FUNC(rb_and), -1);
  rb_define_alias(rb_klass, "&", "and");
  rb_define_method(rb_klass, "or", RUBY_METHOD_FUNC(rb_or), -1);
  rb_define_alias(rb_klass, "|", "or");
  rb_define_method(rb_klass, "xor", RUBY_METHOD_FUNC(rb_xor), -1);
  rb_define_alias(rb_klass, "^", "xor");
  rb_define_method(rb_klass, "not", RUBY_METHOD_FUNC(rb_not), 0);
  rb_define_method(rb_klass, "not!", RUBY_METHOD_FUNC(rb_not_bang), 0);
  rb_define_method(rb_klass, "eq", RUBY_METHOD_FUNC(rb_eq), 1);
  rb_define_method(rb_klass, "gt", RUBY_METHOD_FUNC(rb_gt), 1);
  rb_define_method(rb_klass, "ge", RUBY_METHOD_FUNC(rb_ge), 1);
  rb_define_method(rb_klass, "lt", RUBY_METHOD_FUNC(rb_lt), 1);
  rb_define_method(rb_klass, "le", RUBY_METHOD_FUNC(rb_le), 1);
  rb_define_method(rb_klass, "ne", RUBY_METHOD_FUNC(rb_ne), 1);
  rb_define_method(rb_klass, "in_range", RUBY_METHOD_FUNC(rb_in_range), 2);
  rb_define_method(rb_klass, "abs_diff", RUBY_METHOD_FUNC(rb_abs_diff), 1);
  rb_define_method(rb_klass, "normalize", RUBY_METHOD_FUNC(rb_normalize), -1);
  rb_define_method(rb_klass, "count_non_zero", RUBY_METHOD_FUNC(rb_count_non_zero), 0);
  rb_define_method(rb_klass, "sum", RUBY_METHOD_FUNC(rb_sum), 0);
  rb_define_method(rb_klass, "avg", RUBY_METHOD_FUNC(rb_avg), -1);
  rb_define_method(rb_klass, "avg_sdv", RUBY_METHOD_FUNC(rb_avg_sdv), -1);
  rb_define_method(rb_klass, "sdv", RUBY_METHOD_FUNC(rb_sdv), -1);
  rb_define_method(rb_klass, "min_max_loc", RUBY_METHOD_FUNC(rb_min_max_loc), -1);
  rb_define_method(rb_klass, "dot_product", RUBY_METHOD_FUNC(rb_dot_product), 1);
  rb_define_method(rb_klass, "cross_product", RUBY_METHOD_FUNC(rb_cross_product), 1);
  rb_define_method(rb_klass, "transform", RUBY_METHOD_FUNC(rb_transform), -1);
  rb_define_method(rb_klass, "perspective_transform", RUBY_METHOD_FUNC(rb_perspective_transform), 1);
  rb_define_method(rb_klass, "mul_transposed", RUBY_METHOD_FUNC(rb_mul_transposed), -1);
  rb_define_method(rb_klass, "trace", RUBY_METHOD_FUNC(rb_trace), 0);
  rb_define_method(rb_klass, "transpose", RUBY_METHOD_FUNC(rb_transpose), 0);
  rb_define_alias(rb_klass, "t", "transpose");
  rb_define_method(rb_klass, "det", RUBY_METHOD_FUNC(rb_det), 0);
  rb_define_alias(rb_klass, "determinant", "det");
  rb_define_method(rb_klass, "invert", RUBY_METHOD_FUNC(rb_invert), -1);
  rb_define_method(rb_klass, "solve", RUBY_METHOD_FUNC(rb_solve), -1);
  rb_define_method(rb_klass, "svd", RUBY_METHOD_FUNC(rb_svd), -1);
  rb_define_method(rb_klass, "svbksb", RUBY_METHOD_FUNC(rb_svbksb), -1);
  rb_define_method(rb_klass, "eigenvv", RUBY_METHOD_FUNC(rb_eigenvv), -1);
  rb_define_method(rb_klass, "calc_covar_matrix", RUBY_METHOD_FUNC(rb_calc_covar_matrix), -1);
  rb_define_method(rb_klass, "mahalonobis", RUBY_METHOD_FUNC(rb_mahalonobis), -1);

  /* drawing function */
  rb_define_method(rb_klass, "line", RUBY_METHOD_FUNC(rb_line), -1);
  rb_define_method(rb_klass, "line!", RUBY_METHOD_FUNC(rb_line_bang), -1);
  rb_define_method(rb_klass, "rectangle", RUBY_METHOD_FUNC(rb_rectangle), -1);
  rb_define_method(rb_klass, "rectangle!", RUBY_METHOD_FUNC(rb_rectangle_bang), -1);
  rb_define_method(rb_klass, "circle", RUBY_METHOD_FUNC(rb_circle), -1);
  rb_define_method(rb_klass, "circle!", RUBY_METHOD_FUNC(rb_circle_bang), -1);
  rb_define_method(rb_klass, "ellipse", RUBY_METHOD_FUNC(rb_ellipse), -1);
  rb_define_method(rb_klass, "ellipse!", RUBY_METHOD_FUNC(rb_ellipse_bang), -1);
  rb_define_method(rb_klass, "ellipse_box", RUBY_METHOD_FUNC(rb_ellipse_box), -1);
  rb_define_method(rb_klass, "ellipse_box!", RUBY_METHOD_FUNC(rb_ellipse_box_bang), -1);
  rb_define_method(rb_klass, "fill_poly", RUBY_METHOD_FUNC(rb_fill_poly), -1);
  rb_define_method(rb_klass, "fill_poly!", RUBY_METHOD_FUNC(rb_fill_poly_bang), -1);
  rb_define_method(rb_klass, "fill_convex_poly", RUBY_METHOD_FUNC(rb_fill_convex_poly), -1);
  rb_define_method(rb_klass, "fill_convex_poly!", RUBY_METHOD_FUNC(rb_fill_convex_poly_bang), -1);
  rb_define_method(rb_klass, "poly_line", RUBY_METHOD_FUNC(rb_poly_line), -1);
  rb_define_method(rb_klass, "poly_line!", RUBY_METHOD_FUNC(rb_poly_line_bang), -1);
  rb_define_method(rb_klass, "put_text", RUBY_METHOD_FUNC(rb_put_text), -1);
  rb_define_method(rb_klass, "put_text!", RUBY_METHOD_FUNC(rb_put_text_bang), -1);

  rb_define_method(rb_klass, "dft", RUBY_METHOD_FUNC(rb_dft), -1);
  rb_define_method(rb_klass, "dct", RUBY_METHOD_FUNC(rb_dct), -1);

  rb_define_method(rb_klass, "sobel", RUBY_METHOD_FUNC(rb_sobel), -1);
  rb_define_method(rb_klass, "laplace", RUBY_METHOD_FUNC(rb_laplace), -1);
  rb_define_method(rb_klass, "canny", RUBY_METHOD_FUNC(rb_canny), -1);
  rb_define_method(rb_klass, "pre_corner_detect", RUBY_METHOD_FUNC(rb_pre_corner_detect), -1);
  rb_define_method(rb_klass, "corner_eigenvv", RUBY_METHOD_FUNC(rb_corner_eigenvv), -1);
  rb_define_method(rb_klass, "corner_min_eigen_val", RUBY_METHOD_FUNC(rb_corner_min_eigen_val), -1);
  rb_define_method(rb_klass, "corner_harris", RUBY_METHOD_FUNC(rb_corner_harris), -1);
  rb_define_private_method(rb_klass, "__find_corner_sub_pix", RUBY_METHOD_FUNC(rbi_find_corner_sub_pix), -1);
  rb_define_method(rb_klass, "good_features_to_track", RUBY_METHOD_FUNC(rb_good_features_to_track), -1);

  rb_define_method(rb_klass, "sample_line", RUBY_METHOD_FUNC(rb_sample_line), 2);
  rb_define_method(rb_klass, "rect_sub_pix", RUBY_METHOD_FUNC(rb_rect_sub_pix), -1);
  rb_define_method(rb_klass, "quadrangle_sub_pix", RUBY_METHOD_FUNC(rb_quadrangle_sub_pix), -1);
  rb_define_method(rb_klass, "resize", RUBY_METHOD_FUNC(rb_resize), -1);
  rb_define_method(rb_klass, "warp_affine", RUBY_METHOD_FUNC(rb_warp_affine), -1);
  rb_define_singleton_method(rb_klass, "rotation_matrix2D", RUBY_METHOD_FUNC(rb_rotation_matrix2D), 3);
  rb_define_method(rb_klass, "warp_perspective", RUBY_METHOD_FUNC(rb_warp_perspective), -1);
  rb_define_singleton_method(rb_klass, "find_homography", RUBY_METHOD_FUNC(rb_find_homograpy), -1);
  rb_define_method(rb_klass, "remap", RUBY_METHOD_FUNC(rb_remap), -1);
  rb_define_method(rb_klass, "log_polar", RUBY_METHOD_FUNC(rb_log_polar), -1);

  rb_define_method(rb_klass, "erode", RUBY_METHOD_FUNC(rb_erode), -1);
  rb_define_method(rb_klass, "erode!", RUBY_METHOD_FUNC(rb_erode_bang), -1);
  rb_define_method(rb_klass, "dilate", RUBY_METHOD_FUNC(rb_dilate), -1);
  rb_define_method(rb_klass, "dilate!", RUBY_METHOD_FUNC(rb_dilate_bang), -1);
  rb_define_method(rb_klass, "morphology", RUBY_METHOD_FUNC(rb_morphology), -1);

  rb_define_method(rb_klass, "smooth", RUBY_METHOD_FUNC(rb_smooth), -1);
  rb_define_method(rb_klass, "copy_make_border", RUBY_METHOD_FUNC(rb_copy_make_border), -1);
  rb_define_method(rb_klass, "filter2d", RUBY_METHOD_FUNC(rb_filter2d), -1);
  rb_define_method(rb_klass, "integral", RUBY_METHOD_FUNC(rb_integral), -1);
  rb_define_method(rb_klass, "threshold", RUBY_METHOD_FUNC(rb_threshold), -1);
  rb_define_method(rb_klass, "adaptive_threshold", RUBY_METHOD_FUNC(rb_adaptive_threshold), -1);

  rb_define_method(rb_klass, "pyr_down", RUBY_METHOD_FUNC(rb_pyr_down), -1);
  rb_define_method(rb_klass, "pyr_up", RUBY_METHOD_FUNC(rb_pyr_up), -1);

  rb_define_method(rb_klass, "flood_fill", RUBY_METHOD_FUNC(rb_flood_fill), -1);
  rb_define_method(rb_klass, "flood_fill!", RUBY_METHOD_FUNC(rb_flood_fill_bang), -1);
  rb_define_method(rb_klass, "find_contours", RUBY_METHOD_FUNC(rb_find_contours), -1);
  rb_define_method(rb_klass, "find_contours!", RUBY_METHOD_FUNC(rb_find_contours_bang), -1);
  rb_define_method(rb_klass, "draw_contours", RUBY_METHOD_FUNC(rb_draw_contours), -1);
  rb_define_method(rb_klass, "draw_contours!", RUBY_METHOD_FUNC(rb_draw_contours_bang), -1);
  rb_define_method(rb_klass, "pyr_segmentation", RUBY_METHOD_FUNC(rb_pyr_segmentation), 3);
  rb_define_method(rb_klass, "pyr_mean_shift_filtering", RUBY_METHOD_FUNC(rb_pyr_mean_shift_filtering), -1);
  rb_define_method(rb_klass, "watershed", RUBY_METHOD_FUNC(rb_watershed), 1);

  rb_define_method(rb_klass, "moments", RUBY_METHOD_FUNC(rb_moments), -1);

  rb_define_method(rb_klass, "hough_lines", RUBY_METHOD_FUNC(rb_hough_lines), -1);
  rb_define_method(rb_klass, "hough_circles", RUBY_METHOD_FUNC(rb_hough_circles), -1);

  rb_define_method(rb_klass, "inpaint", RUBY_METHOD_FUNC(rb_inpaint), 3);

  rb_define_method(rb_klass, "equalize_hist", RUBY_METHOD_FUNC(rb_equalize_hist), 0);
  rb_define_method(rb_klass, "match_template", RUBY_METHOD_FUNC(rb_match_template), -1);
  rb_define_method(rb_klass, "match_shapes", RUBY_METHOD_FUNC(rb_match_shapes), -1);

  rb_define_method(rb_klass, "mean_shift", RUBY_METHOD_FUNC(rb_mean_shift), 2);
  rb_define_method(rb_klass, "cam_shift", RUBY_METHOD_FUNC(rb_cam_shift), 2);
  rb_define_method(rb_klass, "snake_image", RUBY_METHOD_FUNC(rb_snake_image), -1);

  rb_define_method(rb_klass, "optical_flow_hs", RUBY_METHOD_FUNC(rb_optical_flow_hs), -1);
  rb_define_method(rb_klass, "optical_flow_lk", RUBY_METHOD_FUNC(rb_optical_flow_lk), 2);
  rb_define_method(rb_klass, "optical_flow_bm", RUBY_METHOD_FUNC(rb_optical_flow_bm), -1);

  rb_define_singleton_method(rb_klass, "find_fundamental_mat",
			     RUBY_METHOD_FUNC(rb_find_fundamental_mat), -1);
  rb_define_singleton_method(rb_klass, "compute_correspond_epilines",
			     RUBY_METHOD_FUNC(rb_compute_correspond_epilines), 3);

  rb_define_method(rb_klass, "extract_surf", RUBY_METHOD_FUNC(rb_extract_surf), -1);

  rb_define_method(rb_klass, "save_image", RUBY_METHOD_FUNC(rb_save_image), 1);
  rb_define_alias(rb_klass, "save", "save_image");

  rb_define_method(rb_klass, "encode_image", RUBY_METHOD_FUNC(rb_encode_imageM), -1);
  rb_define_alias(rb_klass, "encode", "encode_image");
  rb_define_singleton_method(rb_klass, "decode_image", RUBY_METHOD_FUNC(rb_decode_imageM), -1);
  rb_define_alias(rb_singleton_class(rb_klass), "decode", "decode_image");
}


VALUE
rb_allocate(VALUE klass)
{
  return OPENCV_OBJECT(klass, 0);
}

/*
 * call-seq:
 *   CvMat.new(<i>row, col[, depth = CV_8U][, channel = 3]</i>) -> cvmat
 *
 * Create col * row matrix. Each element set 0.
 *
 * Each element possigle range is set by <i>depth</i>. Default is unsigned 8bit.
 *
 * Number of channel is set by <i>channel</i>. <i>channel</i> should be 1..4.
 *
 */
VALUE
rb_initialize(int argc, VALUE *argv, VALUE self)
{
  VALUE row, column, depth, channel;
  rb_scan_args(argc, argv, "22", &row, &column, &depth, &channel);

  int ch = (argc < 4) ? 3 : NUM2INT(channel);
  CvMat *ptr = rb_cvCreateMat(NUM2INT(row), NUM2INT(column),
			      CV_MAKETYPE(CVMETHOD("DEPTH", depth, CV_8U), ch));
  free(DATA_PTR(self));
  DATA_PTR(self) = ptr;

  return self;
}

/*
 * call-seq:
 *   CvMat::load(<i>filename[,iscolor = CV_LOAD_IMAGE_COLOR]</i>)
 *
 * Load an image from file.
 *  iscolor = CV_LOAD_IMAGE_COLOR, the loaded image is forced to be a 3-channel color image
 *  iscolor = CV_LOAD_IMAGE_GRAYSCALE, the loaded image is forced to be grayscale
 *  iscolor = CV_LOAD_IMAGE_UNCHANGED, the loaded image will be loaded as is.
 * Currently the following file format are supported.
 * * Windows bitmaps - BMP,DIB
 * * JPEG files - JPEG,JPG,JPE
 * * Portable Network Graphics - PNG
 * * Portable image format - PBM,PGM,PPM
 * * Sun rasters - SR,RAS
 * * TIFF files - TIFF,TIF
 */
VALUE
rb_load_imageM(int argc, VALUE *argv, VALUE self)
{
  VALUE filename, iscolor;
  rb_scan_args(argc, argv, "11", &filename, &iscolor);
  Check_Type(filename, T_STRING);

  int _iscolor;
  if (NIL_P(iscolor)) {
    _iscolor = CV_LOAD_IMAGE_COLOR;
  }
  else {
    Check_Type(iscolor, T_FIXNUM);
    _iscolor = FIX2INT(iscolor);
  }

  CvMat *mat = NULL;
  try {
    mat = cvLoadImageM(StringValueCStr(filename), _iscolor);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  if (mat == NULL) {
    rb_raise(rb_eStandardError, "file does not exist or invalid format image.");
  }
  return OPENCV_OBJECT(rb_klass, mat);
}

/*
 * call-seq:
 *   encode_image(ext [,params]) -> Array<Integer>
 *
 * Encodes an image into a memory buffer.
 *
 * Parameters:
 *   ext <String> - File extension that defines the output format ('.jpg', '.png', ...)
 *   params <Hash> - Format-specific parameters.
 */
VALUE
rb_encode_imageM(int argc, VALUE *argv, VALUE self)
{
  VALUE _ext, _params;
  rb_scan_args(argc, argv, "11", &_ext, &_params);
  Check_Type(_ext, T_STRING);
  const char* ext = RSTRING_PTR(_ext);
  CvMat* buff = NULL;
  int* params = NULL;

  if (!NIL_P(_params)) {
    Check_Type(_params, T_HASH);
    const int flags[] = {
      CV_IMWRITE_JPEG_QUALITY,
      CV_IMWRITE_PNG_COMPRESSION,
      CV_IMWRITE_PNG_STRATEGY,
      CV_IMWRITE_PNG_STRATEGY_DEFAULT,
      CV_IMWRITE_PNG_STRATEGY_FILTERED,
      CV_IMWRITE_PNG_STRATEGY_HUFFMAN_ONLY,
      CV_IMWRITE_PNG_STRATEGY_RLE,
      CV_IMWRITE_PNG_STRATEGY_FIXED,
      CV_IMWRITE_PXM_BINARY
    };
    const int flag_size = sizeof(flags) / sizeof(int);

    params = ALLOCA_N(int, RHASH_SIZE(_params) * 2);
    for (int i = 0, n = 0; i < flag_size; i++) {
      VALUE val = rb_hash_lookup(_params, INT2FIX(flags[i]));
      if (!NIL_P(val)) {
	params[n] = flags[i];
	params[n + 1] = NUM2INT(val);
	n += 2;
      }
    }
  }

  try {
    buff = cvEncodeImage(ext, CVARR(self), params);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  const int size = buff->rows * buff->cols;
  VALUE array = rb_ary_new2(size);
  for (int i = 0; i < size; i++) {
    rb_ary_store(array, i, CHR2FIX(CV_MAT_ELEM(*buff, char, 0, i)));
  }

  try {
    cvReleaseMat(&buff);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  return array;
}

CvMat*
prepare_decoding(int argc, VALUE *argv, int* iscolor, int* need_release)
{
  VALUE _buff, _iscolor;
  rb_scan_args(argc, argv, "11", &_buff, &_iscolor);
  *iscolor = NIL_P(_iscolor) ? CV_LOAD_IMAGE_COLOR : NUM2INT(_iscolor);

  CvMat* buff = NULL;
  *need_release = 0;
  switch (TYPE(_buff)) {
  case T_STRING:
    _buff = rb_funcall(_buff, rb_intern("unpack"), 1, rb_str_new("c*", 2));
  case T_ARRAY: {
    int cols = RARRAY_LEN(_buff);
    *need_release = 1;
    try {
      buff = rb_cvCreateMat(1, cols, CV_8UC1);
      VALUE *ary_ptr = RARRAY_PTR(_buff);
      for (int i = 0; i < cols; i++) {
	CV_MAT_ELEM(*buff, char, 0, i) = NUM2CHR(ary_ptr[i]);
      }
    }
    catch (cv::Exception& e) {
      raise_cverror(e);
    }
    break;
  }
  case T_DATA:
    if (rb_obj_is_kind_of(_buff, cCvMat::rb_class()) == Qtrue) {
      buff = CVMAT(_buff);
      break;
    }
  default:
    raise_typeerror(_buff, "CvMat, Array or String");
  }

  return buff;
}

/*
 * call-seq:
 *   decode_image(buf[, iscolor=CV_LOAD_IMAGE_COLOR]) -> CvMat
 *
 * Reads an image from a buffer in memory.
 *
 * Parameters:
 *   buf <CvMat, Array, String> - Input array
 *   iscolor <Integer> - Flags specifying the color type of a decoded image (the same flags as CvMat#load)
 */
VALUE
rb_decode_imageM(int argc, VALUE *argv, VALUE self)
{
  int iscolor, need_release;
  CvMat* buff = prepare_decoding(argc, argv, &iscolor, &need_release);
  CvMat* mat_ptr = NULL;
  try {
    mat_ptr = cvDecodeImageM(buff, iscolor);
    if (need_release) {
      cvReleaseMat(&buff);
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  return OPENCV_OBJECT(rb_klass, mat_ptr);
}

/*
 * nodoc
 */
VALUE
rb_method_missing(int argc, VALUE *argv, VALUE self)
{
  VALUE name, args, method;
  rb_scan_args(argc, argv, "1*", &name, &args);
  method = rb_funcall(name, rb_intern("to_s"), 0);
  if (RARRAY_LEN(args) != 0 || !rb_respond_to(rb_module_opencv(), rb_intern(StringValuePtr(method))))
    return rb_call_super(argc, argv);
  return rb_funcall(rb_module_opencv(), rb_intern(StringValuePtr(method)), 1, self);
}

/*
 * call-seq:
 *   to_s -> string
 *
 * Return following string.
 *   m = CvMat.new(100, 100, :cv8u, 3)
 *   m.to_s # => <CvMat:100x100,depth=cv8u,channel=3>
 */
VALUE
rb_to_s(VALUE self)
{
  const int i = 6;
  VALUE str[i];
  str[0] = rb_str_new2("<%s:%dx%d,depth=%s,channel=%d>");
  str[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
  str[2] = rb_width(self);
  str[3] = rb_height(self);
  str[4] = rb_depth(self);
  str[5] = rb_channel(self);
  return rb_f_sprintf(i, str);
}

/*
 * call-seq:
 *   inside?(obj) -> true or false
 *
 *
 */
VALUE
rb_inside_q(VALUE self, VALUE object)
{
  if (cCvPoint::rb_compatible_q(cCvPoint::rb_class(), object)) {
    CvMat *mat = CVMAT(self);
    int x = NUM2INT(rb_funcall(object, rb_intern("x"), 0));
    int y = NUM2INT(rb_funcall(object, rb_intern("y"), 0));
    if (cCvRect::rb_compatible_q(cCvRect::rb_class(), object)) {
      int width = NUM2INT(rb_funcall(object, rb_intern("width"), 0));
      int height = NUM2INT(rb_funcall(object, rb_intern("height"), 0));
      return (x >= 0) && (y >= 0) && (x < mat->width) && ((x + width) < mat->width)
	&& (y < mat->height) && ((y + height) < mat->height) ? Qtrue : Qfalse;
    }
    else {
      return (x >= 0) && (y >= 0) && (x < mat->width) && (y < mat->height) ? Qtrue : Qfalse;
    }
  }
  rb_raise(rb_eArgError, "argument 1 should have method \"x\", \"y\"");
}

/*
 * call-seq:
 *    to_IplConvKernel -> iplconvkernel
 *
 * Create IplConvKernel from this matrix.
 */
VALUE
rb_to_IplConvKernel(VALUE self, VALUE anchor)
{
  CvMat *src = CVMAT(self);
  CvPoint p = VALUE_TO_CVPOINT(anchor);
  IplConvKernel *kernel = rb_cvCreateStructuringElementEx(src->cols, src->rows, p.x, p.y,
							  CV_SHAPE_CUSTOM, src->data.i);
  return DEPEND_OBJECT(cIplConvKernel::rb_class(), kernel, self);
}

/*
 * call-seq:
 *   create_mask -> cvmat(single-channel 8bit unsinged image)
 *
 * Create single-channel 8bit unsinged image that filled 0.
 */
VALUE
rb_create_mask(VALUE self)
{
  VALUE mask = cCvMat::new_object(cvGetSize(CVARR(self)), CV_8UC1);
  try {
    cvZero(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return mask;
}

/*
 * call-seq:
 *   width -> int
 *
 * Return number of columns.
 */
VALUE
rb_width(VALUE self)
{
  return INT2NUM(CVMAT(self)->width);
}

/*
 * call-seq:
 *   height -> int
 *
 * Return number of rows.
 */
VALUE
rb_height(VALUE self)
{
  return INT2NUM(CVMAT(self)->height);
}

/*
 * call-seq:
 *   depth -> symbol
 *
 * Return depth symbol. (see OpenCV::DEPTH)
 */
VALUE
rb_depth(VALUE self)
{
  return rb_hash_aref(rb_funcall(rb_const_get(rb_module_opencv(), rb_intern("DEPTH")), rb_intern("invert"), 0),
		      INT2FIX(CV_MAT_DEPTH(CVMAT(self)->type)));
}

/*
 * call-seq:
 *   channel -> int (1 < channel < 4)
 *
 * Return number of channel.
 */
VALUE
rb_channel(VALUE self)
{
  return INT2FIX(CV_MAT_CN(CVMAT(self)->type));
}

/*
 * call-seq:
 *   data -> binary (by String class)
 *
 * Return raw data of matrix.
 */
VALUE
rb_data(VALUE self)
{
  IplImage *image = IPLIMAGE(self);
  return rb_str_new((char *)image->imageData, image->imageSize);
}

/*
 * call-seq:
 *   clone -> cvmat
 *
 * Clone matrix.
 * Instance-specific method is succeeded.
 *
 *   module M
 *     def example
 *       true
 *     end
 *   end
 *
 *   mat.extend M
 *   mat.example   #=> true
 *   clone = mat.clone
 *   clone.example #=> true
 *   copy = mat.copy
 *   copy.example  #=> raise NoMethodError
 */
VALUE
rb_clone(VALUE self)
{
  VALUE clone = rb_obj_clone(self);
  try {
    DATA_PTR(clone) = cvClone(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return clone;
}

/*
 * call-seq:
 *   copy() -> cvmat
 *   copy(<i>mat</i>) -> mat
 *   copy(<i>val</i>) -> array(include cvmat)
 *
 * Copy matrix.
 * Instance-specific method is *NOT* succeeded. see also #clone.
 *
 * There are 3 kind behavior depending on the argument.
 *
 *   copy()
 *     Return one copied matrix.
 *   copy(mat)
 *     Copy own elements to target matrix. Return nil.
 *     Size (or ROI) and channel and depth should be match.
 *     If own width or height does not match target matrix, raise CvUnmatchedSizes
 *     If own channel or depth does not match target matrix, raise CvUnmatchedFormats
 *   copy(val)
 *     The amounts of the specified number are copied. Return Array with copies.
 *     If you give the 0 or negative value. Return nil.
 *       mat.copy(3)  #=> [mat1, mat2, mat3]
 *       mat.copy(-1) #=> nil
 *
 * When not apply to any, raise ArgumentError
 */
VALUE
rb_copy(int argc, VALUE *argv, VALUE self)
{
  VALUE value, copied;
  CvArr *src = CVARR(self);
  CvSize size = cvGetSize(src);
  rb_scan_args(argc, argv, "01", &value);
  if (argc == 0) {
    copied = new_mat_kind_object(size, self);
    try {
      cvCopy(src, CVARR(copied));
    }
    catch (cv::Exception& e) {
      raise_cverror(e);
    }
    return copied;
  }
  else {
    if (rb_obj_is_kind_of(value, rb_klass)) {
      try {
	cvCopy(src, CVARR_WITH_CHECK(value));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return Qnil;
    }
    else if (FIXNUM_P(value)) {
      int n = FIX2INT(value);
      if (n > 0) {
        copied = rb_ary_new2(n);
        for (int i = 0; i < n; ++i) {
	  VALUE tmp = new_mat_kind_object(size, self);
	  try {
	    cvCopy(src, CVMAT(tmp));
	  }
	  catch (cv::Exception& e) {
	    raise_cverror(e);
	  }
          rb_ary_store(copied, i, tmp);
        }
        return copied;
      }
      else {
        return Qnil;
      }
    }
    else
      rb_raise(rb_eArgError, "Argument should be CvMat or Fixnum");
  }
}

VALUE
copy(VALUE mat)
{
  return rb_clone(mat);
}

inline VALUE
rb_to_X_internal(VALUE self, int depth)
{
  CvMat *src = CVMAT(self);
  VALUE dest = new_object(src->rows, src->cols, CV_MAKETYPE(depth, CV_MAT_CN(src->type)));
  try {
    cvConvert(src, CVMAT(dest));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   to_8u -> cvmat(depth = CV_8U)
 *
 * Return the new matrix that elements is converted to unsigned 8bit.
 */
VALUE
rb_to_8u(VALUE self)
{
  return rb_to_X_internal(self, CV_8U);
}

/*
 * call-seq:
 *   to_8s -> cvmat(depth = CV_8S)
 *
 * Return the new matrix that elements is converted to signed 8bit.
 */
VALUE
rb_to_8s(VALUE self)
{
  return rb_to_X_internal(self, CV_8S);
}

/*
 * call-seq:
 *   to_16u -> cvmat(depth = CV_16U)
 *
 * Return the new matrix that elements is converted to unsigned 16bit.
 */
VALUE rb_to_16u(VALUE self)
{
  return rb_to_X_internal(self, CV_16U);
}

/*
 * call-seq:
 *   to_16s -> cvmat(depth = CV_16s)
 *
 * Return the new matrix that elements is converted to signed 16bit.
 */
VALUE
rb_to_16s(VALUE self)
{
  return rb_to_X_internal(self, CV_16S);
}

/*
 * call-seq:
 *   to_32s -> cvmat(depth = CV_32S)
 *
 * Return the new matrix that elements is converted to signed 32bit.
 */
VALUE
rb_to_32s(VALUE self)
{
  return rb_to_X_internal(self, CV_32S);
}

/*
 * call-seq:
 *   to_32f -> cvmat(depth = CV_32F)
 *
 * Return the new matrix that elements is converted to 32bit floating-point.
 */
VALUE
rb_to_32f(VALUE self)
{
  return rb_to_X_internal(self, CV_32F);
}

/*
 * call-seq:
 *   to_64F -> cvmat(depth = CV_64F)
 *
 * Return the new matrix that elements is converted to 64bit floating-point.
 */
VALUE
rb_to_64f(VALUE self)
{
  return rb_to_X_internal(self, CV_64F);
}

/*
 * call-seq:
 *   vector? -> true or false
 *
 * If #width or #height is 1, return true. Otherwise return false.
 */
VALUE
rb_vector_q(VALUE self)
{
  CvMat *mat = CVMAT(self);
  return (mat->width == 1|| mat->height == 1) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   square? -> true or false
 *
 * If #width == #height return true. Otherwise return false.
 */
VALUE
rb_square_q(VALUE self)
{
  CvMat *mat = CVMAT(self);
  return mat->width == mat->height ? Qtrue : Qfalse;
}

/************************************************************
       cxcore function
************************************************************/
/*
 * Return CvMat object with reference to caller-object.
 *
 *   src = CvMat.new(10, 10)
 *   mat = src.to_CvMat
 *
 * In C, src->data and mat->data are common. Therefore, they cause changes with each other.
 */
VALUE
rb_to_CvMat(VALUE self)
{
  // CvMat#to_CvMat aborts when self's class is CvMat.
  if (CLASS_OF(self) == rb_klass)
    return self;

  CvMat *mat = NULL;
  try {
    mat = cvGetMat(CVARR(self), RB_CVALLOC(CvMat));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return DEPEND_OBJECT(rb_klass, mat, self);
}

/*
 * call-seq:
 *   sub_rect(<i>rect</i>) -> cvmat
 *   sub_rect(<i>topleft</i>, <i>size</i>) -> cvmat
 *   sub_rect(<i>x, y, width, height</i>) -> cvmat
 *
 * Return parts of self as CvMat.
 *
 * <i>p</i> or <i>x</i>,<i>y</i> mean top-left coordinate.
 * <i>size</i> or <i>width</i>,<i>height</i> is size.
 *
 * link:../images/CvMat_sub_rect.png
 */
VALUE
rb_sub_rect(VALUE self, VALUE args)
{
  CvRect area;
  CvPoint topleft;
  CvSize size;
  switch(RARRAY_LEN(args)) {
  case 1:
    area = VALUE_TO_CVRECT(RARRAY_PTR(args)[0]);
    break;
  case 2:
    topleft = VALUE_TO_CVPOINT(RARRAY_PTR(args)[0]);
    size = VALUE_TO_CVSIZE(RARRAY_PTR(args)[1]);
    area.x = topleft.x;
    area.y = topleft.y;
    area.width = size.width;
    area.height = size.height;
    break;
  case 4:
    area.x = NUM2INT(RARRAY_PTR(args)[0]);
    area.y = NUM2INT(RARRAY_PTR(args)[1]);
    area.width = NUM2INT(RARRAY_PTR(args)[2]);
    area.height = NUM2INT(RARRAY_PTR(args)[3]);
    break;
  default:
    rb_raise(rb_eArgError, "wrong number of arguments (%ld of 1 or 2 or 4)", RARRAY_LEN(args));
  }

  CvMat* mat = NULL;
  try {
    mat = cvGetSubRect(CVARR(self), RB_CVALLOC(CvMat), area);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return DEPEND_OBJECT(rb_klass, mat, self);
}

/*
 * call-seq:
 *   get_rows(<i>n</i>)            -> Return row
 *   get_rows(<i>n1, n2, ...</i>)  -> Return Array of row
 *
 * Return row(or rows) of matrix.
 * argument should be Fixnum or CvSlice compatible object.
 */
VALUE
rb_get_rows(VALUE self, VALUE args)
{
  int len = RARRAY_LEN(args);
  if (len < 1)
    rb_raise(rb_eArgError, "wrong number of argument.(more than 1)");
  VALUE ary = rb_ary_new2(len);
  for (int i = 0; i < len; ++i) {
    VALUE value = rb_ary_entry(args, i);
    
    CvMat* row = NULL;
    try {
      if (FIXNUM_P(value))
	row = cvGetRow(CVARR(self), RB_CVALLOC(CvMat), FIX2INT(value));
      else {
	CvSlice slice = VALUE_TO_CVSLICE(value);
	row = cvGetRows(CVARR(self), RB_CVALLOC(CvMat), slice.start_index, slice.end_index);
      }
    }
    catch (cv::Exception& e) {
      if (row != NULL)
	cvReleaseMat(&row);
      raise_cverror(e);
    }
    rb_ary_store(ary, i, DEPEND_OBJECT(rb_klass, row, self));
  }
  return RARRAY_LEN(ary) > 1 ? ary : rb_ary_entry(ary, 0);
}

/*
 * call-seq:
 *   get_cols(<i>n</i>)            -> Return column
 *   get_cols(<i>n1, n2, ...</i>)  -> Return Array of columns
 *
 * Return column(or columns) of matrix.
 * argument should be Fixnum or CvSlice compatible object.
 */
VALUE
rb_get_cols(VALUE self, VALUE args)
{
  int len = RARRAY_LEN(args);
  if (len < 1)
    rb_raise(rb_eArgError, "wrong number of argument.(more than 1)");
  VALUE ary = rb_ary_new2(len);
  for (int i = 0; i < len; ++i) {
    VALUE value = rb_ary_entry(args, i);
    CvMat* col = NULL;
    try {
      if (FIXNUM_P(value))
	col = cvGetCol(CVARR(self), RB_CVALLOC(CvMat), FIX2INT(value));
      else {
	CvSlice slice = VALUE_TO_CVSLICE(value);
	col = cvGetCols(CVARR(self), RB_CVALLOC(CvMat), slice.start_index, slice.end_index);
      }
    }
    catch (cv::Exception& e) {
      if (col != NULL)
	cvReleaseMat(&col);
      raise_cverror(e);
    }
    rb_ary_store(ary, i, DEPEND_OBJECT(rb_klass, col, self));
  }
  return RARRAY_LEN(ary) > 1 ? ary : rb_ary_entry(ary, 0);
}

/*
 * call-seq:
 *   each_row {|row| ... } -> self
 *
 * Calls block once for each row in self, passing that element as a parameter.
 *
 * see also CvMat#each_col
 */
VALUE
rb_each_row(VALUE self)
{
  int rows = CVMAT(self)->rows;
  CvMat* row = NULL;
  for (int i = 0; i < rows; ++i) {
    try {
      row = cvGetRow(CVARR(self), RB_CVALLOC(CvMat), i);
    }
    catch (cv::Exception& e) {
      if (row != NULL)
	cvReleaseMat(&row);
      raise_cverror(e);
    }
    rb_yield(DEPEND_OBJECT(rb_klass, row, self));
  }
  return self;
}

/*
 * call-seq:
 *   each_col {|col| ... } -> self
 *
 * Calls block once for each column in self, passing that element as a parameter.
 *
 * see also CvMat#each_row
 */
VALUE
rb_each_col(VALUE self)
{
  int cols = CVMAT(self)->cols;
  CvMat *col = NULL;
  for (int i = 0; i < cols; ++i) {
    try {
      col = cvGetCol(CVARR(self), RB_CVALLOC(CvMat), i);
    }
    catch (cv::Exception& e) {
      if (col != NULL)
	cvReleaseMat(&col);
      raise_cverror(e);
    }
    rb_yield(DEPEND_OBJECT(rb_klass, col, self));
  }
  return self;
}

/*
 * call-seq:
 *   diag(<i>[val = 0]</i>) -> cvmat
 *
 * Return one of array diagonals.
 * <i>val</i> is zero corresponds to the main diagonal, -1 corresponds to the diagonal above the main etc, 1 corresponds to the diagonal below the main etc.
 *
 */
VALUE
rb_diag(int argc, VALUE *argv, VALUE self)
{
  VALUE val;
  if (rb_scan_args(argc, argv, "01", &val) < 1)
    val = INT2FIX(0);
  CvMat* diag = NULL;
  try {
    diag = cvGetDiag(CVARR(self), RB_CVALLOC(CvMat), NUM2INT(val));
  }
  catch (cv::Exception& e) {
    cvReleaseMat(&diag);
    raise_cverror(e);
  }
  return DEPEND_OBJECT(rb_klass, diag, self);
}

/*
 * call-seq:
 *   size -> cvsize
 *
 * Return size by CvSize
 */
VALUE
rb_size(VALUE self)
{
  CvSize size;
  try {
    size = cvGetSize(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return cCvSize::new_object(size);
}

/*
 * call-seq:
 *   dims -> array(int, int, ...)
 *
 * Return number of array dimensions and their sizes or the size of particular dimension.
 * In case of CvMat it always returns 2 regardless of number of matrix rows.
 */
VALUE
rb_dims(VALUE self)
{
  int size[CV_MAX_DIM];
  int dims = 0;
  try {
    dims = cvGetDims(CVARR(self), size);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  VALUE ary = rb_ary_new2(dims);
  for (int i = 0; i < dims; ++i) {
    rb_ary_store(ary, i, INT2NUM(size[i]));
  }
  return ary;
}

/*
 * call-seq:
 *   dim_size(<i>index</i>) -> int
 *
 * Return number of dimension.
 * almost same as CvMat#dims[<i>index</i>].
 * If the dimension specified with index doesn't exist, CvStatusOutOfRange raise.
 */
VALUE
rb_dim_size(VALUE self, VALUE index)
{
  int dimsize = 0;
  try {
    dimsize = cvGetDimSize(CVARR(self), NUM2INT(index));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return INT2NUM(dimsize);
}

/*
 * call-seq:
 *   [<i>idx1[,idx2]...</i>]
 *
 * Return value of the particular array element as CvScalar.
 */
VALUE
rb_aref(VALUE self, VALUE args)
{
  int index[CV_MAX_DIM];
  for (int i = 0; i < RARRAY_LEN(args); ++i)
    index[i] = NUM2INT(rb_ary_entry(args, i));
  
  CvScalar scalar = cvScalarAll(0);
  try {
    switch (RARRAY_LEN(args)) {
    case 1:
      scalar = cvGet1D(CVARR(self), index[0]);
      break;
    case 2:
      scalar = cvGet2D(CVARR(self), index[0], index[1]);
      break;
    default:
      scalar = cvGetND(CVARR(self), index);
      break;      
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return cCvScalar::new_object(scalar);
}

/*
 * call-seq:
 *   [<i>idx1[,idx2]...</i>] = <i>value</i>
 *
 * Set value of the particular array element to <i>value</i>.
 * <i>value</i> should be CvScalar.
 */
VALUE
rb_aset(VALUE self, VALUE args)
{
  CvScalar scalar = VALUE_TO_CVSCALAR(rb_ary_pop(args));
  int index[CV_MAX_DIM];
  for (int i = 0; i < RARRAY_LEN(args); ++i)
    index[i] = NUM2INT(rb_ary_entry(args, i));

  try {
    switch (RARRAY_LEN(args)) {
    case 1:
      cvSet1D(CVARR(self), index[0], scalar);
      break;
    case 2:
      cvSet2D(CVARR(self), index[0], index[1], scalar);
      break;
    default:
      cvSetND(CVARR(self), index, scalar);
      break;
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   set_data(<i>data</i>)
 *
 * Assigns user data to the array header.
 * <i>data</i> should be Array which contains numbers.
 */
VALUE
rb_set_data(VALUE self, VALUE data)
{
  data = rb_funcall(data, rb_intern("flatten"), 0);
  const int DATA_LEN = RARRAY_LEN(data);
  CvMat *self_ptr = CVMAT(self);
  int depth = CV_MAT_DEPTH(self_ptr->type);
  void* array = NULL;

  switch (depth) {
  case CV_8U:
    array = rb_cvAlloc(sizeof(uchar) * DATA_LEN);
    for (int i = 0; i < DATA_LEN; ++i)
      ((uchar*)array)[i] = (uchar)(NUM2INT(rb_ary_entry(data, i)));
    break;
  case CV_8S:
    array = rb_cvAlloc(sizeof(char) * DATA_LEN);
    for (int i = 0; i < DATA_LEN; ++i)
      ((char*)array)[i] = (char)(NUM2INT(rb_ary_entry(data, i)));
    break;
  case CV_16U:
    array = rb_cvAlloc(sizeof(ushort) * DATA_LEN);
    for (int i = 0; i < DATA_LEN; ++i)
      ((ushort*)array)[i] = (ushort)(NUM2INT(rb_ary_entry(data, i)));
    break;
  case CV_16S:
    array = rb_cvAlloc(sizeof(short) * DATA_LEN);
    for (int i = 0; i < DATA_LEN; ++i)
      ((short*)array)[i] = (short)(NUM2INT(rb_ary_entry(data, i)));
    break;
  case CV_32S:
    array = rb_cvAlloc(sizeof(int) * DATA_LEN);
    for (int i = 0; i < DATA_LEN; ++i)
      ((int*)array)[i] = NUM2INT(rb_ary_entry(data, i));
    break;
  case CV_32F:
    array = rb_cvAlloc(sizeof(float) * DATA_LEN);
    for (int i = 0; i < DATA_LEN; ++i)
      ((float*)array)[i] = (float)NUM2DBL(rb_ary_entry(data, i));
    break;
  case CV_64F:
    array = rb_cvAlloc(sizeof(double) * DATA_LEN);
    for (int i = 0; i < DATA_LEN; ++i)
      ((double*)array)[i] = NUM2DBL(rb_ary_entry(data, i));
    break;
  default:
    rb_raise(rb_eArgError, "Invalid CvMat depth");
    break;
  }

  try {
    cvSetData(self_ptr, array, self_ptr->step);    
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  return self;
}

/*
 * call-seq:
 *   fill(<i>value[, mask]</i>) -> cvmat
 *
 * Return CvMat copied value to every selected element. value should be CvScalar or compatible object.
 *   self[I] = value if mask(I)!=0
 *
 * note: This method support ROI on IplImage class. but COI not support. COI should not be set.
 *   image = IplImage.new(10, 20)         #=> create 3 channel image.
 *   image.coi = 1                        #=> set COI
 *   image.fill(CvScalar.new(10, 20, 30)) #=> raise CvBadCOI error.
 */
VALUE
rb_fill(int argc, VALUE *argv, VALUE self)
{
  return rb_fill_bang(argc, argv, copy(self));
}

/*
 * call-seq:
 *   fill!(<i>value[, mask]</i>) -> self
 *
 * Copie value to every selected element.
 *  self[I] = value if mask(I)!=0
 *
 * see also #fill.
 */
VALUE
rb_fill_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE value, mask;
  rb_scan_args(argc, argv, "11", &value, &mask);
  try {
    cvSet(CVARR(self), VALUE_TO_CVSCALAR(value), MASK(mask));    
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   save_image(<i>filename</i>) -> self
 *
 * Saves an image to file. The image format is chosen depending on the filename extension.
 * Only 8bit single-channel or 3-channel(with 'BGR' channel order) image can be saved.
 *
 * e.g.
 *   image = OpenCV::CvMat.new(10, 10, CV_8U, 3)
 *   image.save_image("image.jpg") #=> save as JPEG format
 *   image.save_image("image.png") #=> save as PNG format
 */
VALUE
rb_save_image(VALUE self, VALUE filename)
{
  Check_Type(filename, T_STRING);
  try {
    cvSaveImage(StringValueCStr(filename), CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   clear -> cvmat
 *
 * Return new matrix all element-value cleared.
 */
VALUE
rb_clear(VALUE self)
{
  return rb_clear_bang(copy(self));
}

/*
 * call-seq:
 *  clear! -> self
 *
 * Clear all element-value. Return self.
 */
VALUE
rb_clear_bang(VALUE self)
{
  try {
    cvSetZero(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   identity(<i>[val = [1]]</i>) -> cvmat
 *
 * Return initializes scaled identity matrix.
 * <i>val</i> should be CvScalar.
 *
 *  arr(i, j) = val if i = j, 0 otherwise
 */
VALUE
rb_set_identity(int argc, VALUE *argv, VALUE self)
{
  return rb_set_identity_bang(argc, argv, copy(self));
}

/*
 * call-seq:
 *   identity!(<i>[val = [1]]</i>) -> self
 *
 * Initialize scaled identity matrix.
 * <i>val</i> should be CvScalar.
 *
 *  arr(i, j) = val if i = j, 0 otherwise
 */
VALUE
rb_set_identity_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE val;
  CvScalar value;
  if (rb_scan_args(argc, argv, "01",  &val) < 1)
    value = cvRealScalar(1);
  else
    value = VALUE_TO_CVSCALAR(val);

  try {
    cvSetIdentity(CVARR(self), value);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   range(start, end) -> cvmat
 *
 * Create and return filled matrix with given range of numbers.
 *
 * see range!
 */
VALUE
rb_range(VALUE self, VALUE start, VALUE end)
{
  return rb_range_bang(copy(self), start, end);
}

/*
 * call-seq:
 *   range!(start, end) -> self
 *
 * Fills matrix with given range of numbers.
 *
 * initializes the matrix as following:
 *   arr(i,j)=(end-start)*(i*cols(arr)+j)/(cols(arr)*rows(arr))
 * For example, the following code will initilize 1D vector with subsequent integer numbers.
 *   m = CvMat.new(1, 10, :cv32s)
 *   m.range!(0, m.cols);            // m will be initialized as [0,1,2,3,4,5,6,7,8,9]
 */
VALUE
rb_range_bang(VALUE self, VALUE start, VALUE end)
{
  try {
    cvRange(CVARR(self), NUM2DBL(start), NUM2DBL(end));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}


/*
 * call-seq:
 *   reshape(<i>[:rows => num][, :channel => cn]</i>) -> cvmat(refer self)
 *
 * Change shape of matrix/image without copying data.
 *
 * e.g.
 *  mat = CvMat.new(3, 3, CV_8U, 3)  #=> 3x3 3-channel matrix
 *  vec = mat.reshape(:rows => 1)    #=> 1x9 3-channel matrix
 *  ch1 = mat.reshape(:channel => 1) #=> 9x9 1-channel matrix
 */
VALUE
rb_reshape(VALUE self, VALUE hash)
{
  Check_Type(hash, T_HASH);
  VALUE channel = rb_hash_aref(hash, ID2SYM(rb_intern("channel")));
  VALUE rows = rb_hash_aref(hash, ID2SYM(rb_intern("rows")));
  CvMat *mat = NULL;
  try {
    mat = cvReshape(CVARR(self), RB_CVALLOC(CvMat), NIL_P(channel) ? 0 : NUM2INT(channel),
		    NIL_P(rows) ? 0 : NUM2INT(rows));
  }
  catch (cv::Exception& e) {
    if (mat != NULL)
      cvReleaseMat(&mat);
    raise_cverror(e);
  }
  return DEPEND_OBJECT(rb_klass, mat, self);
}

/*
 * call-seq:
 *   repeat(<i>mat</i>) -> cvmat
 *
 * Tiled <i>mat</i> by self.
 */
VALUE
rb_repeat(VALUE self, VALUE object)
{
  try {
    cvRepeat(CVARR(self), CVARR_WITH_CHECK(object));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return object;
}

/*
 * call-seq:
 *   flip(:x)  -> cvmat
 *   flip(:y)  -> cvmat
 *   flip(:xy) -> cvmat
 *   flip      -> cvmat
 *
 * Return new flipped 2D array.
 * * flip(:x)  - flip around horizontal
 * * flip(:y)  - flip around vertical
 * * flip(:xy) - flip around both axises
 * * flip      - flip around vertical
 */
VALUE
rb_flip(int argc, VALUE *argv, VALUE self)
{
  return rb_flip_bang(argc, argv, copy(self));
}

/*
 * call-seq:
 *   flip!(:x)  -> self
 *   flip!(:y)  -> self
 *   flip!(:xy) -> self
 *   flip!      -> self
 *
 * Flip 2D array. Return self.
 *
 * see also CvMat#flip
 */
VALUE
rb_flip_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE format;
  int mode = 1;
  if (rb_scan_args(argc, argv, "01", &format) > 0) {
    Check_Type(format, T_SYMBOL);
    ID flip_mode = rb_to_id(format);
    if (flip_mode == rb_intern("x")) {
      mode = 1;
    }
    else if (flip_mode == rb_intern("y")) {
      mode = 0;
    }
    else if (flip_mode == rb_intern("xy")) {
      mode = -1;
    }
  }
  try {
    cvFlip(CVARR(self), NULL, mode);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   split -> array(include cvmat)
 *
 * Divides multi-channel array into several single-chanel arrays.
 *
 * e.g.
 *  image = CvMat.new 640, 480, CV_8U, 3 #=> 3-channel image
 *  image.split                          #=> [image1, image2, image3] : each image have single-channel
 *
 * e.g. switch red <-> blue channel.
 *  image = IplImage.load "sample.bmp"
 *  i = image.split
 *  new_image = CvMat.merge i[2], i[1], i[0]
 */
VALUE
rb_split(VALUE self)
{
  CvArr* self_ptr = CVARR(self);
  int type = cvGetElemType(self_ptr);
  int depth = CV_MAT_DEPTH(type), channel = CV_MAT_CN(type);
  VALUE dest = rb_ary_new2(channel);
  try {
    CvArr *dest_ptr[] = { NULL, NULL, NULL, NULL };
    CvSize size = cvGetSize(self_ptr);
    for (int i = 0; i < channel; ++i) {
      VALUE tmp = new_mat_kind_object(size, self, depth, 1);
      rb_ary_store(dest, i, tmp);
      dest_ptr[i] = CVARR(tmp);
    }
    cvSplit(self_ptr, dest_ptr[0], dest_ptr[1], dest_ptr[2], dest_ptr[3]);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  return dest;
}

/*
 * call-seq:
 *   CvMat.merge(<i>mat1[,mat2][,mat3][,mat4]</i>) -> cvmat
 *
 * Composes multi-channel array from several single-channel arrays.
 * Each argument should be single-channel image(CvMat or subclass).
 * All image should be same size and same depth.
 *
 * see also CvMat#split
 */
VALUE
rb_merge(VALUE klass, VALUE args)
{
  int len = RARRAY_LEN(args);
  if (len <= 0 || len > 4) {
    rb_raise(rb_eArgError, "wrong number of argument (%d for 1..4)", len);
  }
  CvMat *src[] = { NULL, NULL, NULL, NULL }, *prev_src = NULL;
  for (int i = 0; i < len; ++i) {
    VALUE object = rb_ary_entry(args, i);
    if (NIL_P(object))
      src[i] = NULL;
    else {
      src[i] = CVMAT_WITH_CHECK(object);
      if (CV_MAT_CN(src[i]->type) != 1)
        rb_raise(rb_eArgError, "image should be single-channel CvMat.");
      if (prev_src == NULL)
        prev_src = src[i];
      else {
        if (!CV_ARE_SIZES_EQ(prev_src, src[i]))
          rb_raise(rb_eArgError, "image size should be same.");
        if (!CV_ARE_DEPTHS_EQ(prev_src, src[i]))
          rb_raise(rb_eArgError, "image depth should be same.");
      }
    }
  }
  // TODO: adapt IplImage
  VALUE dest = Qnil;
  try {
    dest = new_object(cvGetSize(src[0]), CV_MAKETYPE(CV_MAT_DEPTH(src[0]->type), len));
    cvMerge(src[0], src[1], src[2], src[3], CVARR(dest));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   rand_shuffle([seed = nil][,iter_factor = 1]) -> cvmat
 *
 * Return shuffled matrix
 *
 * see rand_shuffle!
 */
VALUE
rb_rand_shuffle(int argc, VALUE *argv, VALUE self)
{
  return rb_rand_shuffle_bang(argc, argv, copy(self));
}

/*
 * call-seq:
 *   rand_shuffle!([seed = nil][,iter_factor = 1]) -> cvmat
 *
 * Shuffles the matrix by swapping randomly chosen pairs of the matrix elements on each iteration
 * (where each element may contain several components in case of multi-channel arrays). The number of
 * iterations (i.e. pairs swapped) is (iter_factor*mat.rows*mat.cols).round, so iter_factor=0 means
 * that no shuffling is done, iter_factor=1 means that the function swaps rows(mat)*cols(mat) random
 * pairs etc.
 */
VALUE
rb_rand_shuffle_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE seed, iter;
  rb_scan_args(argc, argv, "02", &seed, &iter);
  try {
    if (NIL_P(seed))
      cvRandShuffle(CVARR(self), NULL, IF_INT(iter, 1));
    else {
      CvRNG rng = cvRNG(rb_num2ll(seed));
      cvRandShuffle(CVARR(self), &rng, IF_INT(iter, 1));
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   lut(<i>lookup_table</i>) -> cvmat
 *
 * Return new matrix performed lookup-table transform.
 *
 * <i>lookup_table</i> should be CvMat that have 256 element (e.g. 1x256 matrix).
 * Otherwise, raise CvStatusBadArgument error.
 *
 * And <i>lookup_table</i> should either have a single-channel, or the same number of channels.
 * When single-channel lookup-table given, same table is used for all channels.
 */
VALUE
rb_lut(VALUE self, VALUE lut)
{
  VALUE dest = copy(self);
  try {
    cvLUT(CVARR(self), CVARR(dest), CVARR_WITH_CHECK(lut));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   convert_scale(<i>:depth => nil, :scale => 1.0, :shift => 0.0</i>)
 *
 * Return new array with optional linear transformation.
 *   mat(I) = src(I) * scale + (shift, shift, ...)
 */
VALUE
rb_convert_scale(VALUE self, VALUE hash)
{
  Check_Type(hash, T_HASH);
  CvMat* self_ptr = CVMAT(self);
  VALUE depth = rb_hash_aref(hash, ID2SYM(rb_intern("depth"))),
    scale = rb_hash_aref(hash, ID2SYM(rb_intern("scale"))),
    shift = rb_hash_aref(hash, ID2SYM(rb_intern("shift")));

  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self,
			       CVMETHOD("DEPTH", depth, CV_MAT_DEPTH(self_ptr->type)),
			       CV_MAT_CN(self_ptr->type));
    cvConvertScale(self_ptr, CVARR(dest), IF_DBL(scale, 1.0), IF_DBL(shift, 0.0));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   convert_scale_abs(<i>:scale => 1.0, :shift => 0.0</i>)
 *
 * Return new array with optional linear transformation.
 * It is similar to CvMat#convert_scale, but it stores absolute values of the conversion result
 *   mat(I) = (src(I) * scale + (shift, shift, ...)).abs
 */
VALUE
rb_convert_scale_abs(VALUE self, VALUE hash)
{
  Check_Type(hash, T_HASH);
  CvMat* self_ptr = CVMAT(self);
  VALUE scale = rb_hash_aref(hash, ID2SYM(rb_intern("scale"))),
    shift = rb_hash_aref(hash, ID2SYM(rb_intern("shift")));
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_8U, CV_MAT_CN(CVMAT(self)->type));
    cvConvertScaleAbs(self_ptr, CVARR(dest), IF_DBL(scale, 1.0), IF_DBL(shift, 0.0));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   add(<i>val[,mask]</i>) -> cvmat
 *
 * Return new matrix computed per-element sum.
 * <i>val</i> should be CvMat or CvScalar.
 * If <i>val</i> is CvMat, it must have same type (depth and channel).
 * <i>mask</i> should be CvMat(8bit single-channel).
 * For each element (I)
 *  dst(I) = src1(I) + src2(I) if mask(I) != 0
 */
VALUE
rb_add(int argc, VALUE *argv, VALUE self)
{
  VALUE val, mask, dest;
  rb_scan_args(argc, argv, "11", &val, &mask);
  dest = copy(self);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvAdd(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
    else
      cvAddS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   sub(<i>val[,mask]</i>) -> cvmat
 *
 * Return new matrix computed per-element difference.
 * <i>val</i> should be CvMat or CvScalar.
 * If <i>val</i> is CvMat, it must have same type (depth and channel).
 * <i>mask</i> should be CvMat(8bit single-channel).
 * For each element (I)
 *  dst(I) = src1(I) - src2(I) if mask(I) != 0
 */
VALUE
rb_sub(int argc, VALUE *argv, VALUE self)
{
  VALUE val, mask, dest;
  rb_scan_args(argc, argv, "11", &val, &mask);
  dest = copy(self);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvSub(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
    else
      cvSubS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   mul(<i>val[,scale = 1.0]</i>) -> cvmat
 *
 * Return new matrix computed per-element product.
 * <i>val</i> should be CvMat or CvScalar.
 * If <i>val</i> is CvMat, it must have same type (depth and channel).
 * For each element (I)
 *  dst(I) = scale * src1(I) * src2(I)
 */
VALUE
rb_mul(int argc, VALUE *argv, VALUE self)
{
  VALUE val, scale, dest;
  if (rb_scan_args(argc, argv, "11", &val, &scale) < 2)
    scale = rb_float_new(1.0);
  dest = new_mat_kind_object(cvGetSize(CVARR(self)), self);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvMul(CVARR(self), CVARR(val), CVARR(dest), NUM2DBL(scale));
    else {
      CvScalar scl = VALUE_TO_CVSCALAR(val);
      VALUE mat = new_object(cvGetSize(CVARR(self)), cvGetElemType(CVARR(self)));
      cvSet(CVARR(mat), scl);
      cvMul(CVARR(self), CVARR(mat), CVARR(dest), NUM2DBL(scale));
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   mat_mul(<i>val[,shiftvec]</i>) -> cvmat
 * Performs matrix multiplication
 *   dst = src1 * src2 + shiftvec
 * <i>val</i> and <i>shiftvec</i> should be CvMat
 * All the matrices should have the same data type and coordinated sizes.
 * Real or complex floating-point matrices are supported.
 */
VALUE
rb_mat_mul(int argc, VALUE *argv, VALUE self)
{
  VALUE val, shiftvec, dest;
  rb_scan_args(argc, argv, "11", &val, &shiftvec);
  CvArr* self_ptr = CVARR(self);  
  dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  try {
    if (NIL_P(shiftvec))
      cvMatMul(self_ptr, CVARR_WITH_CHECK(val), CVARR(dest));
    else
      cvMatMulAdd(self_ptr, CVARR_WITH_CHECK(val), CVARR_WITH_CHECK(shiftvec), CVARR(dest));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   div(<i>val[,scale = 1.0]</i>) -> cvmat
 *
 * Return new matrix computed per-element division.
 * <i>val</i> should be CvMat or CvScalar.
 * If <i>val</i> is CvMat, it must have same type (depth and channel).
 * For each element (I)
 *  dst(I) = scale * src1(I) / src2(I)
 */
VALUE
rb_div(int argc, VALUE *argv, VALUE self)
{
  VALUE val, scale;
  if (rb_scan_args(argc, argv, "11", &val, &scale) < 2)
    scale = rb_float_new(1.0);
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    if (rb_obj_is_kind_of(val, rb_klass))
      cvDiv(self_ptr, CVARR(val), CVARR(dest), NUM2DBL(scale));
    else {
      CvScalar scl = VALUE_TO_CVSCALAR(val);
      VALUE mat = new_mat_kind_object(cvGetSize(self_ptr), self);
      CvArr* mat_ptr = CVARR(mat);
      cvSet(mat_ptr, scl);
      cvDiv(self_ptr, mat_ptr, CVARR(dest), NUM2DBL(scale));
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   add_weighted(src1, alpha, src2, beta, gamma)
 *
 * Computes the weighted sum of two arrays.
 *
 * src1 - The first source array
 * alpha - Weight for the first array elements
 * src2 - The second source array
 * beta - Weight for the second array elements
 * gamma - Added to each sum
 *
 * The function calculates the weighted sum of two arrays as follows:
 *   dst(I)=src1(I)*alpha+src2(I)*beta+gamma
 * All the arrays must have the same type and the same size (or ROI size). 
 * For types that have limited range this operation is saturating.
 */
VALUE
rb_add_weighted(VALUE klass, VALUE src1, VALUE alpha, VALUE src2, VALUE beta, VALUE gamma)
{
  CvArr* src1_ptr = CVARR_WITH_CHECK(src1);
  VALUE dst = new_mat_kind_object(cvGetSize(src1_ptr), src1);
  try {
    cvAddWeighted(src1_ptr, NUM2DBL(alpha),
		  CVARR_WITH_CHECK(src2), NUM2DBL(beta),
		  NUM2DBL(gamma), CVARR(dst));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dst;
}

/*
 * call-seq:
 *   and(<i>val[,mask]</i>) -> cvmat
 *
 * Return new matrix computed per-element bit-wise conjunction.
 * <i>val</i> should be CvMat or CvScalar.
 * If <i>val</i> is CvMat, it must have same type (depth and channel).
 * For each element (I)
 *  dst(I) = src1(I) & src2(I) if mask(I) != 0
 */
VALUE
rb_and(int argc, VALUE *argv, VALUE self)
{
  VALUE val, mask, dest;
  rb_scan_args(argc, argv, "11", &val, &mask);
  dest = copy(self);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvAnd(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
    else
      cvAndS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   or(<i>val[,mask]</i>) -> cvmat
 *
 * Return new matrix computed per-element bit-wise disjunction.
 * <i>val</i> should be CvMat or CvScalar.
 * If <i>val</i> is CvMat, it must have same type (depth and channel).
 * For each element (I)
 *  dst(I) = src1(I) | src2(I) if mask(I) != 0
 */
VALUE
rb_or(int argc, VALUE *argv, VALUE self)
{
  VALUE val, mask, dest;
  rb_scan_args(argc, argv, "11", &val, &mask);
  dest = copy(self);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvOr(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
    else
      cvOrS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   xor(<i>val[,mask]</i>) -> cvmat
 *
 * Return new matrix computed per-element bit-wise "exclusive or" operation.
 * <i>val</i> should be CvMat or CvScalar.
 * If <i>val</i> is CvMat, it must have same type (depth and channel).
 * For each element (I)
 *  dst(I) = src1(I) ^ src2(I) if mask(I) != 0
 */
VALUE
rb_xor(int argc, VALUE *argv, VALUE self)
{
  VALUE val, mask, dest;
  rb_scan_args(argc, argv, "11", &val, &mask);
  dest = copy(self);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvXor(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
    else
      cvXorS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   not -> cvmat
 *
 * Return new matrix performed per-element bit-wise inversion.
 *  dst(I) =~ src(I)
 */
VALUE
rb_not(VALUE self)
{
  return rb_not_bang(copy(self));
}

/*
 * call-seq:
 *   not! -> self
 *
 * Performe per-element bit-wise inversion.
 */
VALUE
rb_not_bang(VALUE self)
{
  try {
    cvNot(CVARR(self), CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

VALUE
rb_cmp_internal(VALUE self, VALUE val, int operand)
{
  CvArr* self_ptr = CVARR(self);
  VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_8U, 1);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvCmp(self_ptr, CVARR(val), CVARR(dest), operand);
    else if (CV_MAT_CN(cvGetElemType(self_ptr)) == 1 && rb_obj_is_kind_of(val, rb_cNumeric))
      cvCmpS(self_ptr, NUM2DBL(val), CVARR(dest), operand);
    else {
      VALUE mat = new_mat_kind_object(cvGetSize(CVARR(self)), self);
      cvSet(CVARR(mat), VALUE_TO_CVSCALAR(val));
      cvCmp(self_ptr, CVARR(mat), CVARR(dest), operand);
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   eq(<i>val</i>) -> cvmat
 *
 * Return new matrix performed per-element comparision "equal".
 *  dst(I) = (self(I) == val(I) ? 0xFF : 0)
 */
VALUE
rb_eq(VALUE self, VALUE val)
{
  return rb_cmp_internal(self, val, CV_CMP_EQ);
}

/*
 * call-seq:
 *   gt(<i>val</i>) -> cvmat
 *
 * Return new matrix performed per-element comparision "greater than".
 *  dst(I) = (self(I) > val(I) ? 0xFF : 0)
 */
VALUE
rb_gt(VALUE self, VALUE val)
{
  return rb_cmp_internal(self, val, CV_CMP_GT);
}

/*
 * call-seq:
 *   ge(<i>val</i>) -> cvmat
 *
 * Return new matrix performed per-element comparision "greater or equal".
 *  dst(I) = (self(I) >= val(I) ? 0xFF : 0)
 */
VALUE
rb_ge(VALUE self, VALUE val)
{
  return rb_cmp_internal(self, val, CV_CMP_GE);
}

/*
 * call-seq:
 *   lt(<i>val</i>) -> cvmat
 *
 * Return new matrix performed per-element comparision "less than".
 *  dst(I) = (self(I) < val(I) ? 0xFF : 0)
 */
VALUE
rb_lt(VALUE self, VALUE val)
{
  return rb_cmp_internal(self, val, CV_CMP_LT);
}

/*
 * call-seq:
 *   le(<i>val</i>) -> cvmat
 *
 * Return new matrix performed per-element comparision "less or equal".
 *  dst(I) = (self(I) <= val(I) ? 0xFF : 0)
 */
VALUE
rb_le(VALUE self, VALUE val)
{
  return rb_cmp_internal(self, val, CV_CMP_LE);
}

/*
 * call-seq:
 *   ne(<i>val</i>) -> cvmat
 *
 * Return new matrix performed per-element comparision "not equal".
 *  dst(I) = (self(I) != val(I) ? 0xFF : 0)
 */
VALUE
rb_ne(VALUE self, VALUE val)
{
  return rb_cmp_internal(self, val, CV_CMP_NE);
}

/*
 * call-seq:
 *   in_range(<i>min, max</i>) -> cvmat
 *
 * Check that element lie between two object.
 * <i>min</i> and <i>max</i> should be CvMat that have same size and type, or CvScalar.
 * Return new matrix performed per-element,
 *  dst(I) = within the range ? 0xFF : 0
 */
VALUE
rb_in_range(VALUE self, VALUE min, VALUE max)
{
  CvArr* self_ptr = CVARR(self);
  CvSize size = cvGetSize(self_ptr);
  VALUE dest = new_object(size, CV_8UC1);
  try {
    if (rb_obj_is_kind_of(min, rb_klass) && rb_obj_is_kind_of(max, rb_klass))
      cvInRange(self_ptr, CVARR(min), CVARR(max), CVARR(dest));
    else if (rb_obj_is_kind_of(min, rb_klass)) {
      VALUE tmp = new_object(size, cvGetElemType(self_ptr));
      cvSet(CVARR(tmp), VALUE_TO_CVSCALAR(max));
      cvInRange(self_ptr, CVARR(min), CVARR(tmp), CVARR(dest));
    }
    else if (rb_obj_is_kind_of(max, rb_klass)) {
      VALUE tmp = new_object(size, cvGetElemType(self_ptr));
      cvSet(CVARR(tmp), VALUE_TO_CVSCALAR(min));
      cvInRange(self_ptr, CVARR(tmp), CVARR(max), CVARR(dest));
    }
    else
      cvInRangeS(self_ptr, VALUE_TO_CVSCALAR(min), VALUE_TO_CVSCALAR(max), CVARR(dest));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   abs_diff(<i>val</i>) -> cvmat
 *
 * Calculate absolute difference between two.
 * <i>val</i> should be CvMat that have same size and same type, or CvScalar.
 *  dst(I) = (src(I) - val(I)).abs
 */
VALUE
rb_abs_diff(VALUE self, VALUE val)
{
  CvArr* self_ptr = CVARR(self);
  VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  try {
    if (rb_obj_is_kind_of(val, rb_klass))
      cvAbsDiff(self_ptr, CVARR(val), CVARR(dest));
    else
      cvAbsDiffS(self_ptr, CVARR(dest), VALUE_TO_CVSCALAR(val));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   normalize(...) -> cvmat
 *
 * Normalizes the norm or value range of an array
 */
VALUE
rb_normalize(int argc, VALUE *argv, VALUE self)
{
  VALUE alphaVal, betaVal, normTypeVal;
  rb_scan_args(argc, argv, "03", &alphaVal, &betaVal, &normTypeVal);
  
  const double alpha = alphaVal != Qnil ? NUM2DBL(alphaVal) : 1.0;
  const double beta = betaVal != Qnil ? NUM2DBL(betaVal) : 0.0;
  const int normType = normTypeVal != Qnil ? NUM2INT(normTypeVal) : cv::NORM_L2;
  
  VALUE dest = new_mat_kind_object(cvGetSize(CVARR(self)), self);
  try {
    const cv::Mat selfMat(CVMAT(self));
    cv::Mat destMat(CVMAT(dest));
    
    cv::normalize(selfMat, destMat, alpha, beta, normType);
    
  } catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   count_non_zero -> int
 *
 * Returns the number of non-zero elements.
 *   result = sumI arr(I)!=0
 *
 * In case of IplImage both ROI and COI are supported.
 */
VALUE
rb_count_non_zero(VALUE self)
{
  int n = 0;
  try {
    n = cvCountNonZero(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return INT2NUM(n);
}

/*
 * call-seq:
 *   sum -> scalar
 *
 * Return summerizes elements as CvScalar. Independently for each channel.
 *
 * note: If COI is setted in IplImage, the method processes the selected channel only and store the sum to the first component scalar[0].
 */
VALUE
rb_sum(VALUE self)
{
  CvScalar sum;
  try {
    sum = cvSum(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return cCvScalar::new_object(sum);
}

/*
 * call-seq:
 *   avg(<i>[mask]</i>) -> mean(as scalar)
 *
 * Return the average(mean) of elements as CvScalar. Independently for each channel.
 */
VALUE
rb_avg(int argc, VALUE *argv, VALUE self)
{
  VALUE mask;
  rb_scan_args(argc, argv, "01", &mask);
  CvScalar avg;
  try {
    avg = cvAvg(CVARR(self), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return cCvScalar::new_object(avg);
}

/*
 * call-seq:
 *   avg_sdv(<i>mask</i>) -> [mean(as scalar), std_dev(as scalar)]
 *
 * Calculates the average value and standard deviation of array elements, independently for each channel.
 *
 * note: same as [CvMat#avg, CvMat#sdv]
 */
VALUE
rb_avg_sdv(int argc, VALUE *argv, VALUE self)
{
  VALUE mask, mean, std_dev;
  rb_scan_args(argc, argv, "01", &mask);
  mean = cCvScalar::new_object();
  std_dev = cCvScalar::new_object();
  try {
    cvAvgSdv(CVARR(self), CVSCALAR(mean), CVSCALAR(std_dev), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(2, mean, std_dev);
}

/*
 * call-seq:
 *   sdv(<i>[mask]</i>) -> std_dev(as scalar)
 *
 * Return the standard deviation of elements as CvScalar. Independently for each channel.
 */
VALUE
rb_sdv(int argc, VALUE *argv, VALUE self)
{
  VALUE mask, std_dev;
  rb_scan_args(argc, argv, "01", &mask);
  std_dev = cCvScalar::new_object();
  try {
    cvAvgSdv(CVARR(self), NULL, CVSCALAR(std_dev), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return std_dev;
}

/*
 * call-seq:
 *   min_max_loc(<i>[mask]</i>) -> [min_val, max_val, min_loc(as point), max_loc(as point)]
 *
 * Finds minimum and maximum element values and their positions.
 * The extremums are searched over the whole array, selected ROI(in case of IplImage) or, if mask is not NULL, in the specified array region.
 * If the array has more than one channel, it must be IplImage with COI set.
 * In case if multi-dimensional arrays min_loc.x and max_loc.x will contain raw (linear) positions of the extremums.
 */
VALUE
rb_min_max_loc(int argc, VALUE *argv, VALUE self)
{
  VALUE mask, min_loc, max_loc;
  double min_val = 0.0, max_val = 0.0;
  rb_scan_args(argc, argv, "01", &mask);
  min_loc = cCvPoint::new_object();
  max_loc = cCvPoint::new_object();
  try {
    cvMinMaxLoc(CVARR(self), &min_val, &max_val, CVPOINT(min_loc), CVPOINT(max_loc), MASK(mask));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(4, rb_float_new(min_val), rb_float_new(max_val), min_loc, max_loc);
}

/*
 * call-seq:
 *   dot_product(<i>mat</i>) -> float
 *
 * Calculates dot product of two arrays in Euclidian metrics.
 * <i>mat</i> should be CvMat have same size and same type.
 *
 *  src1.src2 = sum(src1(I) * src2(I))
 */
VALUE
rb_dot_product(VALUE self, VALUE mat)
{
  double result = 0.0;
  try {
    result = cvDotProduct(CVARR(self), CVARR_WITH_CHECK(mat));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_float_new(result);
}

/*
 * call-seq:
 *   cross_product(<i>mat</i>) -> cvmat
 *
 * Calculate cross product of two 3D vectors.
 * <i>mat</i> should be CvMat have same size and same type.
 */
VALUE
rb_cross_product(VALUE self, VALUE mat)
{
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvCrossProduct(self_ptr, CVARR_WITH_CHECK(mat), CVARR(dest));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   transform(<i>transmat[,shiftvec]</i>) -> cvmat
 *
 * performs matrix transform of every element.
 *  dst(I) = transmat * src(I) + shiftvec
 */
VALUE
rb_transform(int argc, VALUE *argv, VALUE self)
{
  VALUE transmat, shiftvec;
  rb_scan_args(argc, argv, "11", &transmat, &shiftvec);
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvTransform(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(transmat),
		NIL_P(shiftvec) ? NULL : CVMAT_WITH_CHECK(shiftvec));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   perspective_transform(<i>mat</i>) -> cvmat
 *
 * Return performed perspective matrix transform of vector array.
 * <i>mat</i> should be 3x3 or 4x4 transform matrix (CvMat).
 * Every element (by treating it as 2D or 3D vector) in the following way:
 *   (x, y, z) -> (x'/w, y'/w, z'/w) or
 *   (x, y) -> (x'/w, y'/w)
 * where
 *   (x', y', z', w') = mat4x4*(x, y, z, 1) or
 *   (x', y', w') = mat3x3*(x, y, 1)
 * and
 *   w = w' if w'!=0, inf otherwise.
 */
VALUE
rb_perspective_transform(VALUE self, VALUE mat)
{
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvPerspectiveTransform(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(mat));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *  mul_transposed(<i>:order => 0 or 1, :delta => cvmat, :scale => number</i>)
 *
 * Calculates the product of self and its transposition.
 *
 * options
 * * :order -> should be 0 or 1 (default is 0)
 *    see below.
 * * :delta -> should be CvMat (default is nil)
 *     An optional array, subtracted from source before multiplication.
 * * :scale -> should be a number (default is 1.0)
 *     An optional scaling
 *
 * mul_transposed evaluates:
 *   :order => 0
 *     dst = scale * (self - delta) * (self - delta)T
 *   :order => 1
 *     dst = scale * (self - delta)T * (self - delta)
 *
 */
VALUE
rb_mul_transposed(int argc, VALUE *argv, VALUE self)
{
  VALUE options = Qnil;
  VALUE _delta = Qnil, _scale = Qnil, _order = Qnil;

  if (rb_scan_args(argc, argv, "01", &options) > 0) {
    Check_Type(options, T_HASH);
    _delta = LOOKUP_CVMETHOD(options, "delta");
    _scale = LOOKUP_CVMETHOD(options, "scale");
    _order = LOOKUP_CVMETHOD(options, "order");
  }

  CvArr* delta = NIL_P(_delta) ? NULL : CVARR_WITH_CHECK(_delta);
  double scale = NIL_P(_scale) ? 1.0 : NUM2DBL(_scale);
  int order = NIL_P(_order) ? 0 : NUM2INT(_order);
  CvArr* self_ptr = CVARR(self);
  VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  try {
    cvMulTransposed(self_ptr, CVARR(dest), order, delta, scale);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  return dest;
}


/*
 * call-seq:
 *   trace -> scalar
 *
 * Returns trace of matrix. "trace" is sum of diagonal elements of the matrix.
 */
VALUE
rb_trace(VALUE self)
{
  CvScalar scalar;
  try {
    scalar = cvTrace(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return cCvScalar::new_object(scalar);
}

/*
 * call-seq:
 *   transpose -> cvmat
 *
 * Return transposed matrix.
 */
VALUE
rb_transpose(VALUE self)
{
  CvMat* self_ptr = CVMAT(self);
  VALUE dest = new_mat_kind_object(cvSize(self_ptr->rows, self_ptr->cols), self);
  try {
    cvTranspose(self_ptr, CVARR(dest));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   det -> float
 *
 * Return determinant of matrix.
 * <i>self</i> should be single-channel and floating-point depth.
 */
VALUE
rb_det(VALUE self)
{
  double det = 0.0;
  try {
    det = cvDet(CVARR(self));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_float_new(det);
}

/*
 * call-seq:
 *   invert(<i>inversion_method=:lu[,delta]</i>) -> float
 *
 * Finds inverse or pseudo-inverse of matrix.
 * <i>inversion_method</i> should be following symbol.
 * * :lu
 *     Gaussian elimincation with optimal pivot element chose.
 *     Return self determinant (self must be square).
 * * :svd
 *     Singular value decomposition(SVD) method.
 *     Return the inversed condition number of self(ratio of the smallest singular value to the largest singular value)
 *     and 0 if self is all zeros. The SVD method calculate a pseudo-inverse matrix if self is singular.
 * * :svd_sym or :svd_symmetric
 *     SVD method for a symmetric positively-defined matrix.
 *
 * <i>self</i> type should be single-channel and floating-point matrix.
 */
VALUE
rb_invert(int argc, VALUE *argv, VALUE self)
{
  VALUE symbol;
  rb_scan_args(argc, argv, "01", &symbol);
  int method = CVMETHOD("INVERSION_METHOD", symbol, CV_LU);
  VALUE dest = Qnil;
  CvArr* self_ptr = CVARR(self);
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvInvert(self_ptr, CVARR(dest), method);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   solve(<i>mat, inversion_method=:lu</i>)
 *
 * Solves linear system or least-squares problem (the latter is possible with SVD method).
 *
 * <i>inversion_method</i> should be following symbol.
 * * :lu
 *     Gaussian elimincation with optimal pivot element chose.
 *     Return self determinant (self must be square).
 * * :svd
 *     Singular value decomposition(SVD) method.
 *     Return the inversed condition number of self(ratio of the smallest singular value to the largest singular value)
 *     and 0 if self is all zeros. The SVD method calculate a pseudo-inverse matrix if self is singular.
 * * :svd_sym or :svd_symmetric
 *     SVD method for a symmetric positively-defined matrix.
 */
VALUE
rb_solve(int argc, VALUE *argv, VALUE self)
{
  VALUE mat, symbol;
  rb_scan_args(argc, argv, "11", &mat, &symbol);
  VALUE dest = Qnil;
  CvArr* arr_ptr = CVARR_WITH_CHECK(mat);
  try {
    dest = new_mat_kind_object(cvGetSize(arr_ptr), self);
    cvSolve(CVARR(self), arr_ptr, CVARR(dest), CVMETHOD("INVERSION_METHOD", symbol, CV_LU));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   svd(<i>l[flag=0]</i>)
 *
 * Performs singular value decomposition of real floating-point matrix.
 */
VALUE
rb_svd(int argc, VALUE *argv, VALUE self)
{
  VALUE _flag = Qnil;
  int flag = 0;
  if (rb_scan_args(argc, argv, "01", &_flag) > 0) {
    flag = NUM2INT(_flag);
  }

  CvMat* self_ptr = CVMAT(self);
  VALUE w = new_mat_kind_object(cvSize(self_ptr->cols, self_ptr->rows), self);
  
  int rows = 0;
  int cols = 0;
  if (flag & CV_SVD_U_T) {
    rows = MIN(self_ptr->rows, self_ptr->cols);
    cols = self_ptr->rows;
  }
  else {
    rows = self_ptr->rows;
    cols = MIN(self_ptr->rows, self_ptr->cols);
  }
  VALUE u = new_mat_kind_object(cvSize(cols, rows), self);

  if (flag & CV_SVD_V_T) {
    rows = MIN(self_ptr->rows, self_ptr->cols);
    cols = self_ptr->cols;
  }
  else {
    rows = self_ptr->cols;
    cols = MIN(self_ptr->rows, self_ptr->cols);
  }
  VALUE v = new_mat_kind_object(cvSize(cols, rows), self);

  cvSVD(self_ptr, CVARR(w), CVARR(u), CVARR(v), flag);

  return rb_ary_new3(3, w, u, v);
}

/*
 * call-seq:
 *   svbksb
 *
 * not yet.
 */
VALUE
rb_svbksb(int argc, VALUE *argv, VALUE self)
{
  rb_raise(rb_eNotImpError, "");
}

/*
 * call-seq:
 *   eigenvv!(<i>[eps = 0.0]</i>) -> [eigen_vectors(cvmat), eigen_values(cvmat)]
 *
 * Computes eigenvalues and eigenvectors of symmetric matrix.
 * <i>self</i> should be symmetric square matrix. <i>self</i> is modified during the processing.
 *
 *   self * eigen_vectors(i,:)' = eigen_values(i) * eigen_vectors(i,:)'
 *
 * Currently the function is slower than #svd yet less accurate, so if <i>self</i> is known to be positively-defined
 * (e.g., it is a convariation matrix), it is recommanded to use #svd to find eigenvalues and eigenvectors of <i>self</i>,
 * especially if eigenvectors are not required.
 */
VALUE
rb_eigenvv(int argc, VALUE *argv, VALUE self)
{
  VALUE epsilon, lowindex, highindex;
  rb_scan_args(argc, argv, "03", &epsilon, &lowindex, &highindex);
  double eps = (NIL_P(epsilon)) ? 0.0 : NUM2DBL(epsilon);
  int lowidx = (NIL_P(lowindex)) ? -1 : NUM2INT(lowindex);
  int highidx = (NIL_P(highindex)) ? -1 : NUM2INT(highindex);
  VALUE eigen_vectors = Qnil, eigen_values = Qnil;
  CvArr* self_ptr = CVARR(self);
  try {
    CvSize size = cvGetSize(self_ptr);
    int type = cvGetElemType(self_ptr);
    eigen_vectors = new_object(size, type);
    eigen_values = new_object(size.height, 1, type);
    cvEigenVV(self_ptr, CVARR(eigen_vectors), CVARR(eigen_values), eps, lowidx, highidx);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(2, eigen_vectors, eigen_values);
}

/*
 * call-seq:
 *   calc_covar_matrix()
 *
 * not yet.
 *
 */
VALUE
rb_calc_covar_matrix(int argc, VALUE *argv, VALUE self)
{
  rb_raise(rb_eNotImpError, "");
}

/*
 * call-seq:
 *   mahalonobis(vec, mat) -> float
 *
 * not yet.
 */
VALUE
rb_mahalonobis(int argc, VALUE *argv, VALUE self)
{
  rb_raise(rb_eNotImpError, "");
}


/*
 * call-seq:
 *   dft(<i>anyflags...</i>) -> cvmat
 *
 * Performs forward or inverse Discrete Fourier Transform(DFT) of 1D or 2D floating-point array.
 * Argument should be following symbol or combination of these.
 *
 * * :forward or :inverse
 *     Do forward or inverse transform. The result is not scaled.
 * * :scale
 *     Scale the result: divide it by the number of array elements.
 * * :rows
 *     Do forward or inverse transform of every individual row of the self.
 *     This flag allow user to transofrm multiple vectors simulaneously and can be used to decrease the overhand
 *     (which sometimes several times larger then the processing itself), to do 3D and higher-dimensional transforms etc.
 *
 * e.g.
 *   mat.dft(:inverse)
 *   mat.dft(:forward, :scale)  etc...
 */
VALUE
rb_dft(int argc, VALUE *argv, VALUE self)
{
  int type = CV_DXT_FORWARD;
  int num_rows = 0;
  if (argc > 0) {
    int num_flags = argc;
    if (FIXNUM_P(argv[argc -1])) {
      num_flags = argc - 1;
      num_rows = FIX2INT(argv[argc - 1]);
    }
    for (int i = 0; i < num_flags; ++i) {
      type |= CVMETHOD("DXT_FLAG", argv[i]);
    }
  }
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvDFT(self_ptr, CVARR(dest), type, num_rows);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   dct(<i>anyflags...</i>) -> cvmat
 *
 * Performs forward or inverse Discrete Cosine Transform(DCT) of 1D or 2D floating-point array.
 * Argument should be following symbol or combination of these.
 *
 * * :forward or :inverse
 *     Do forward or inverse transform.
 * * :rows
 *     Do forward or inverse transform of every individual row of the self.
 *     This flag allow user to transofrm multiple vectors simulaneously and can be used to decrease the overhand
 *     (which sometimes several times larger then the processing itself), to do 3D and higher-dimensional transforms etc.
 */
VALUE
rb_dct(int argc, VALUE *argv, VALUE self)
{
  int type = CV_DXT_FORWARD;
  if (argc > 0) {
    for (int i = 0; i < argc; ++i) {
      type |= CVMETHOD("DXT_FLAG", argv[i]);
    }
  }
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvDCT(self_ptr, CVARR(dest), type);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   line(<i>p1, p2[, drawing_option]</i>) -> mat
 *
 * Return image is drawn a line segment connecting two points.
 *
 * <i>drawing_option</i> should be Hash include these keys.
 *   :color
 *      Line color.
 *   :thickness
 *      Line Thickness.
 *   :line_type
 *      Type of the line:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the point coordinates.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 *
 * for example
 *   mat = CvMat.new(100, 100)
 *   mat.line(CvPoint.new(10, 10), CvPoint.new(90, 90), :thickness => 3, :line_type => :aa)
 */
VALUE
rb_line(int argc, VALUE *argv, VALUE self)
{
  return rb_line_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   line!(<i>p1, p2[, drawing_option]</i>) -> self
 *
 * Draws a line segment connecting two points.
 * Same as CvMat#line, but modifies the receiver in place.
 * see CvMat#line
 */
VALUE
rb_line_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE p1, p2, drawing_option;
  rb_scan_args(argc, argv, "21", &p1, &p2, &drawing_option);
  drawing_option = DRAWING_OPTION(drawing_option);
  try {
    cvLine(CVARR(self), VALUE_TO_CVPOINT(p1), VALUE_TO_CVPOINT(p2),
	   DO_COLOR(drawing_option),
	   DO_THICKNESS(drawing_option),
	   DO_LINE_TYPE(drawing_option),
	   DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   rectangle(<i>p1, p2[, drawing_option]</i>) -> mat
 *
 * Return image is drawn a rectangle with two opposite corners <i>p1</i> and <i>p2</i>.
 *
 * <i>drawing_options</i> should be Hash include these keys.
 *   :color
 *      Line color.
 *   :thickness
 *      Thickness of lines that make up the rectangle.
 *      Negative values make the function to draw a filled rectangle.
 *   :line_type
 *      Type of the line:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the point coordinates.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 */
VALUE
rb_rectangle(int argc, VALUE *argv, VALUE self)
{
  return rb_rectangle_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   rectangle!(<i>p1, p2[, drawing_option]</i>) -> self
 *
 * Draws simple, thick or filled rectangle.
 * Same as CvMat#rectangle, but modifies the receiver in place.
 * see CvMat#rectangle
 */
VALUE
rb_rectangle_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE p1, p2, drawing_option;
  rb_scan_args(argc, argv, "21", &p1, &p2, &drawing_option);
  drawing_option = DRAWING_OPTION(drawing_option);
  try {
    cvRectangle(CVARR(self), VALUE_TO_CVPOINT(p1), VALUE_TO_CVPOINT(p2),
		DO_COLOR(drawing_option),
		DO_THICKNESS(drawing_option),
		DO_LINE_TYPE(drawing_option),
		DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   circle(<i>center, radius[,drawing_option]</i>) -> cvmat
 *
 * Return image is drawn a simple or filled circle with given center and radius.
 *
 * <i>drawing_options</i> should be Hash include these keys.
 *   :color
 *      Circle color.
 *   :thickness
 *      Thickness of the circle outline if positive, otherwise that a filled circle has to be drawn.
 *   :line_type
 *      Type of the circle boundary:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the center coordinates and radius value.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 */
VALUE
rb_circle(int argc, VALUE *argv, VALUE self)
{
  return rb_circle_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   circle!(<i>center, radius[,drawing_option]</i>) -> cvmat
 *
 * Draw a circle.
 * Same as CvMat#circle, but modifies the receiver in place.
 *
 * see CvMat#circle
 */
VALUE
rb_circle_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE center, radius, drawing_option;
  rb_scan_args(argc, argv, "21", &center, &radius, &drawing_option);
  drawing_option = DRAWING_OPTION(drawing_option);
  try {
    cvCircle(CVARR(self), VALUE_TO_CVPOINT(center), NUM2INT(radius),
	     DO_COLOR(drawing_option),
	     DO_THICKNESS(drawing_option),
	     DO_LINE_TYPE(drawing_option),
	     DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   ellipse(<i>center, axis, angle, start_angle, end_angle[,drawing_option]</i>) -> mat
 *
 * Return image is drawn a simple or thick elliptic arc or fills an ellipse sector.
 *
 * <i>drawing_options</i> should be Hash include these keys.
 *   :color
 *      Ellipse color.
 *   :thickness
 *      Thickness of the ellipse arc.
 *   :line_type
 *      Type of the ellipse boundary:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the center coordinates and axes' value.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 */
VALUE
rb_ellipse(int argc, VALUE *argv, VALUE self)
{
  return rb_ellipse_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   ellipse!(<i>center, axis, angle, start_angle, end_angle[,drawing_option]</i>) -> self
 *
 * Draws simple or thick elliptic arc or fills ellipse sector.
 * Same as CvMat#ellipse, but modifies the receiver in place.
 *
 * see CvMat#ellipse
 */
VALUE
rb_ellipse_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE center, axis, angle, start_angle, end_angle, drawing_option;
  rb_scan_args(argc, argv, "51", &center, &axis, &angle, &start_angle, &end_angle, &drawing_option);
  drawing_option = DRAWING_OPTION(drawing_option);
  try {
    cvEllipse(CVARR(self), VALUE_TO_CVPOINT(center),
	      VALUE_TO_CVSIZE(axis),
	      NUM2DBL(angle), NUM2DBL(start_angle), NUM2DBL(end_angle),
	      DO_COLOR(drawing_option),
	      DO_THICKNESS(drawing_option),
	      DO_LINE_TYPE(drawing_option),
	      DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   ellipse_box(<i>box[, drawing_option]</i>) -> mat
 *
 * Return image is drawn a simple or thick ellipse outline, or fills an ellipse.
 * The method provides a convenient way to draw an ellipse approximating some shape.
 *
 * <i>drawing_options</i> should be Hash include these keys.
 *   :color
 *      Ellipse color.
 *   :thickness
 *      Thickness of the ellipse drawn.
 *   :line_type
 *      Type of the ellipse boundary:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the box vertex coordinates.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 */
VALUE
rb_ellipse_box(int argc, VALUE *argv, VALUE self)
{
  return rb_ellipse_box_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   ellipse_box!(<i>box[, drawing_option]</i>) -> self
 *
 * Draws simple or thick elliptic arc or fills ellipse sector.
 * Same as CvMat#ellipse_box, but modifies the receiver in place.
 *
 * see CvMat#ellipse_box
 */
VALUE
rb_ellipse_box_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE box, drawing_option;
  rb_scan_args(argc, argv, "11", &box, &drawing_option);
  drawing_option = DRAWING_OPTION(drawing_option);
  try {
    cvEllipseBox(CVARR(self), VALUE_TO_CVBOX2D(box),
		 DO_COLOR(drawing_option),
		 DO_THICKNESS(drawing_option),
		 DO_LINE_TYPE(drawing_option),
		 DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   fill_poly(<i>points[,drawing_option]</i>) -> mat
 *
 * Return image is filled an area bounded by several polygonal contours.
 * The method fills complex areas, for example, areas with holes, contour self-intersection, etc.
 */
VALUE
rb_fill_poly(int argc, VALUE *argv, VALUE self)
{
  return rb_fill_poly_bang(argc, argv, self);
}

/*
 * call-seq:
 *   fill_poly!(<i>points[,drawing_option]</i>) -> self
 *
 * Fills polygons interior.
 * Same as CvMat#fill_poly, but modifies the receiver in place.
 *
 * drawing_options should be Hash include these keys.
 *   :color
 *      Polygon color.
 *   :line_type
 *      Type of the polygon boundaries:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the vertex coordinates.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 */
VALUE
rb_fill_poly_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE polygons, drawing_option;
  VALUE points;
  int i, j;
  int num_polygons;
  int *num_points;
  CvPoint **p;

  rb_scan_args(argc, argv, "11", &polygons, &drawing_option);
  Check_Type(polygons, T_ARRAY);
  drawing_option = DRAWING_OPTION(drawing_option);
  num_polygons = RARRAY_LEN(polygons);
  num_points = ALLOCA_N(int, num_polygons);

  p = ALLOCA_N(CvPoint*, num_polygons);
  for (j = 0; j < num_polygons; ++j) {
    points = rb_ary_entry(polygons, j);
    Check_Type(points, T_ARRAY);
    num_points[j] = RARRAY_LEN(points);
    p[j] = ALLOCA_N(CvPoint, num_points[j]);
    for (i = 0; i < num_points[j]; ++i) {
      p[j][i] = VALUE_TO_CVPOINT(rb_ary_entry(points, i));
    }
  }
  try {
    cvFillPoly(CVARR(self), p, num_points, num_polygons,
	       DO_COLOR(drawing_option),
	       DO_LINE_TYPE(drawing_option),
	       DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   fill_convex_poly(<i>points[,drawing_option]</i>) -> mat
 *
 * Return image is filled convex polygon interior.
 * This method is much faster than The function CvMat#fill_poly
 * and can fill not only the convex polygons but any monotonic polygon,
 * i.e. a polygon whose contour intersects every horizontal line (scan line)
 * twice at the most.
 *
 * <i>drawing_options</i> should be Hash include these keys.
 *   :color
 *      Polygon color.
 *   :line_type
 *      Type of the polygon boundaries:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the vertex coordinates.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 */
VALUE
rb_fill_convex_poly(int argc, VALUE *argv, VALUE self)
{
  return rb_fill_convex_poly_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   fill_convex_poly!(<i>points[,drawing_option]</i>) -> self
 *
 * Fills convex polygon.
 * Same as CvMat#fill_convex_poly, but modifies the receiver in place.
 *
 * see CvMat#fill_convex_poly
 */
VALUE
rb_fill_convex_poly_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE points, drawing_option;
  int i, num_points;
  CvPoint *p;

  rb_scan_args(argc, argv, "11", &points, &drawing_option);
  Check_Type(points, T_ARRAY);
  drawing_option = DRAWING_OPTION(drawing_option);
  num_points = RARRAY_LEN(points);
  p = ALLOCA_N(CvPoint, num_points);
  for (i = 0; i < num_points; ++i)
    p[i] = VALUE_TO_CVPOINT(rb_ary_entry(points, i));

  try {
    cvFillConvexPoly(CVARR(self), p, num_points,
		     DO_COLOR(drawing_option),
		     DO_LINE_TYPE(drawing_option),
		     DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   poly_line(<i>points[,drawing_option]</i>) -> mat
 *
 * Return image drawed a single or multiple polygonal curves.
 *
 * <i>drawing_option</i> should be Hash include these keys.
 *   :is_closed
 *      Indicates whether the polylines must be drawn closed.
 *      If closed, the method draws the line from the last vertex
 *      of every contour to the first vertex.
 *   :color
 *      Polyline color.
 *   :thickness
 *      Thickness of the polyline edges
 *   :line_type
 *      Type of line segments:
 *        * 0 or 8 - 8-connected line(default).
 *        * 4 - 4-connected line.
 *        * negative-value - antialiased line.
 *   :shift
 *      Number of fractional bits in the vertex coordinates.
 *
 * note: <i>drawing_option</i>'s default value is CvMat::DRAWING_OPTION.
 */
VALUE
rb_poly_line(int argc, VALUE *argv, VALUE self)
{
  return rb_poly_line_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   poly_line!(<i>points[,drawing_option]</i>) -> self
 *
 * Draws simple or thick polygons.
 *
 * Same as CvMat#poly_line, but modifies the receiver in place.
 *
 * see CvMat#poly_line
 */
VALUE
rb_poly_line_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE polygons, drawing_option;
  VALUE points;
  int i, j;
  int num_polygons;
  int *num_points;
  CvPoint **p;

  rb_scan_args(argc, argv, "11", &polygons, &drawing_option);
  Check_Type(polygons, T_ARRAY);
  drawing_option = DRAWING_OPTION(drawing_option);
  num_polygons = RARRAY_LEN(polygons);
  num_points = ALLOCA_N(int, num_polygons);
  p = ALLOCA_N(CvPoint*, num_polygons);

  for (j = 0; j < num_polygons; ++j) {
    points = rb_ary_entry(polygons, j);
    Check_Type(points, T_ARRAY);
    num_points[j] = RARRAY_LEN(points);
    p[j] = ALLOCA_N(CvPoint, num_points[j]);
    for (i = 0; i < num_points[j]; ++i) {
      p[j][i] = VALUE_TO_CVPOINT(rb_ary_entry(points, i));
    }
  }

  try {
    cvPolyLine(CVARR(self), p, num_points, num_polygons,
	       DO_IS_CLOSED(drawing_option),
	       DO_COLOR(drawing_option),
	       DO_THICKNESS(drawing_option),
	       DO_LINE_TYPE(drawing_option),
	       DO_SHIFT(drawing_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  return self;
}


/*
 * call-seq:
 *   put_text(<i>str, point, font[,color]</i>) -> cvmat
 *
 * Return image is drawn text string.
 * <i>font</i> should be CvFont object.
 */
VALUE
rb_put_text(int argc, VALUE* argv, VALUE self)
{
  return rb_put_text_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   put_text!(<i>str, point, font[,color]</i>) -> self
 *
 * Draws text string. Return self.
 */
VALUE
rb_put_text_bang(int argc, VALUE* argv, VALUE self)
{
  VALUE _text, _point, _font, _color;
  rb_scan_args(argc, argv, "31", &_text, &_point, &_font, &_color);
  CvScalar color = NIL_P(_color) ? CV_RGB(0, 0, 0) : VALUE_TO_CVSCALAR(_color);
  try {
    cvPutText(CVARR(self), StringValueCStr(_text), VALUE_TO_CVPOINT(_point),
	      CVFONT_WITH_CHECK(_font), color);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   sobel(<i>xorder,yorder[,aperture_size=3]</i>) -> cvmat
 *
 * Calculates first, second, third or mixed image derivatives using extended Sobel operator.
 * <i>self</i> should be single-channel 8bit unsigned or 32bit floating-point.
 *
 * link:../images/CvMat_sobel.png
 */
VALUE
rb_sobel(int argc, VALUE *argv, VALUE self)
{
  VALUE xorder, yorder, aperture_size, dest;
  if (rb_scan_args(argc, argv, "21", &xorder, &yorder, &aperture_size) < 3)
    aperture_size = INT2FIX(3);
  CvMat* self_ptr = CVMAT(self);
  switch(CV_MAT_DEPTH(self_ptr->type)) {
  case CV_8U:
    dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_16S, 1);
    break;
  case CV_32F:
    dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_32F, 1);
    break;
  default:
    rb_raise(rb_eArgError, "source depth should be CV_8U or CV_32F.");
    break;
  }

  try {
    cvSobel(self_ptr, CVARR(dest), NUM2INT(xorder), NUM2INT(yorder), NUM2INT(aperture_size));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   laplace(<i>[aperture_size = 3]</i>) -> cvmat
 *
 * Calculates Laplacian of the image.
 * <i>self</i> should be single-channel 8bit unsigned or 32bit floating-point.
 */
VALUE
rb_laplace(int argc, VALUE *argv, VALUE self)
{
  VALUE aperture_size, dest;
  if (rb_scan_args(argc, argv, "01", &aperture_size) < 1)
    aperture_size = INT2FIX(3);
  CvMat* self_ptr = CVMAT(self);
  switch(CV_MAT_DEPTH(self_ptr->type)) {
  case CV_8U:
    dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_16S, 1);
    break;
  case CV_32F:
    dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_32F, 1);
    break;
  default:
    rb_raise(rb_eArgError, "source depth should be CV_8U or CV_32F.");
  }

  try {
    cvLaplace(self_ptr, CVARR(dest), NUM2INT(aperture_size));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   canny(<i>thresh1,thresh2[,aperture_size = 3]</i>) -> cvmat
 *
 * Canny algorithm for edge detection.
 */
VALUE
rb_canny(int argc, VALUE *argv, VALUE self)
{
  VALUE thresh1, thresh2, aperture_size;
  if (rb_scan_args(argc, argv, "21", &thresh1, &thresh2, &aperture_size) < 3)
    aperture_size = INT2FIX(3);
  CvArr* self_ptr = CVARR(self);
  VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  
  try {
    cvCanny(self_ptr, CVARR(dest), NUM2INT(thresh1), NUM2INT(thresh2), NUM2INT(aperture_size));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   pre_corner_detect(<i>[aperture_size = 3]</i>) -> cvmat
 *
 * Calculates feature map for corner detection.
 * <i>aperture_size</i> is parameter for sobel operator(see #sobel).
 *
 * The corners can be found as local maximums of the function.
 */
VALUE
rb_pre_corner_detect(int argc, VALUE *argv, VALUE self)
{
  VALUE aperture_size, dest = Qnil;
  if (rb_scan_args(argc, argv, "01", &aperture_size) < 1)
    aperture_size = INT2FIX(3);

  CvArr *self_ptr = CVARR(self);
  try {
    dest = new_object(cvGetSize(self_ptr), CV_MAKETYPE(CV_32F, 1));
    cvPreCornerDetect(self_ptr, CVARR(dest), NUM2INT(aperture_size));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   corner_eigenvv(<i>block_size[,aperture_size]</i>) -> cvmat
 *
 * For every pixel considers <i>block_size x block_size</i> neighborhood S(p).
 * It calculates convariation matrix of derivatives over the neighborhood.
 */
VALUE
rb_corner_eigenvv(int argc, VALUE *argv, VALUE self)
{
  VALUE block_size, aperture_size, dest;
  if (rb_scan_args(argc, argv, "11", &block_size, &aperture_size) < 2)
    aperture_size = INT2FIX(3);
  CvMat* self_ptr = CVMAT(self);
  dest = new_object(cvSize(self_ptr->cols * 6, self_ptr->rows), CV_MAKETYPE(CV_32F, 1));
  try {
    cvCornerEigenValsAndVecs(self_ptr, CVARR(dest), NUM2INT(block_size), NUM2INT(aperture_size));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   corner_min_eigen_val(<i>block_size[,aperture_size = 3]</i>) -> cvmat
 *
 * Calculates minimal eigenvalue of gradient matrices for corner detection.
 */
VALUE
rb_corner_min_eigen_val(int argc, VALUE *argv, VALUE self)
{
  VALUE block_size, aperture_size, dest;
  if (rb_scan_args(argc, argv, "11", &block_size, &aperture_size) < 2)
    aperture_size = INT2FIX(3);
  CvArr* self_ptr = CVARR(self);
  dest = new_object(cvGetSize(self_ptr), CV_MAKETYPE(CV_32F, 1));
  try {
    cvCornerMinEigenVal(self_ptr, CVARR(dest), NUM2INT(block_size), NUM2INT(aperture_size));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   corner_harris(<i>block_size[,aperture_size = 3][,k = 0.04]</i>) -> cvmat
 *
 * Return image Applied Harris edge detector.
 */
VALUE
rb_corner_harris(int argc, VALUE *argv, VALUE self)
{
  VALUE block_size, aperture_size, k, dest;
  rb_scan_args(argc, argv, "12", &block_size, &aperture_size, &k);
  CvArr* self_ptr = CVARR(self);
  dest = new_object(cvGetSize(self_ptr), CV_MAKETYPE(CV_32F, 1));
  try {
    cvCornerHarris(self_ptr, CVARR(dest), NUM2INT(block_size), IF_INT(aperture_size, 3), IF_DBL(k, 0.04));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   find_corner_sub_pix(<i></i>)
 *
 * Refines corner locations.
 * This method iterates to find the sub-pixel accurate location of corners,
 * or radial saddle points, as shown in on the picture below.
 */
VALUE
rbi_find_corner_sub_pix(int argc, VALUE *argv, VALUE self)
{
  /*
    VALUE corners, win, zero_zone, criteria;
    rb_scan_args(argc, argv, "13", &corners, &win, &zero_zone, &criteria);
    if (!rb_obj_is_kind_of(corners, mPointSet::rb_module()))
    rb_raise(rb_eTypeError, "argument 1 (corners) should be %s.", rb_class2name(mPointSet::rb_module()));
    int count = CVSEQ(corners)->total;
    VALUE storage = cCvMemStorage::new_object();
    CvPoint2D32f *pointset = POINTSET2D32f(corners);
    //cvFindCornerSubPix(CVARR(self), pointset, count, VALUE_TO_CVSIZE(win), VALUE_TO_CVSIZE(zero_zone), VALUE_TO_CVTERMCRITERIA(criteria));
    //return cCvSeq::new_sequence();
    */
  return Qnil;
}

/*
 * call-seq:
 *   good_features_to_track(<i>quality_level, min_distance[, good_features_to_track_option]</i>)
 *         -> array (include CvPoint2D32f)
 * Determines strong corners on an image.
 *
 * quality_level – Multiplier for the max/min eigenvalue; specifies the minimal accepted quality of image corners
 * min_distance – Limit, specifying the minimum possible distance between the returned corners; Euclidian distance is used
 * <i>good_features_to_track_option</i> should be Hash include these keys.
 *   :mask
 *      Region of interest. The function selects points either in the specified region or in the whole image
 *      if the mask is nil.
 *   :block_size
 *      Size of the averaging block, passed to the underlying CornerMinEigenVal or CornerHarris used by the function.
 *   :use_harris
 *      If true, Harris operator ( CornerHarris ) is used instead of default CornerMinEigenVal
 *   :k
 *      Free parameter of Harris detector; used only if ( :use_harris => true )
 * note: <i>good_features_to_track_option</i>'s default value is CvMat::GOOD_FEATURES_TO_TRACK_OPTION
 */
VALUE
rb_good_features_to_track(int argc, VALUE *argv, VALUE self)
{
  VALUE quality_level, min_distance, good_features_to_track_option;
  rb_scan_args(argc, argv, "21", &quality_level, &min_distance, &good_features_to_track_option);
  good_features_to_track_option = GOOD_FEATURES_TO_TRACK_OPTION(good_features_to_track_option);
  int np = GF_MAX(good_features_to_track_option);
  if (np <= 0)
    rb_raise(rb_eArgError, "option :max should be positive value.");

  CvMat *self_ptr = CVMAT(self);
  CvPoint2D32f *p32 = (CvPoint2D32f*)rb_cvAlloc(sizeof(CvPoint2D32f) * np);
  int type = CV_MAKETYPE(CV_32F, 1);
  CvMat* eigen = rb_cvCreateMat(self_ptr->rows, self_ptr->cols, type);
  CvMat* tmp = rb_cvCreateMat(self_ptr->rows, self_ptr->cols, type);
  try {
    cvGoodFeaturesToTrack(self_ptr, &eigen, &tmp, p32, &np, NUM2DBL(quality_level), NUM2DBL(min_distance),
			  GF_MASK(good_features_to_track_option),
			  GF_BLOCK_SIZE(good_features_to_track_option),
			  GF_USE_HARRIS(good_features_to_track_option),
			  GF_K(good_features_to_track_option));
  }
  catch (cv::Exception& e) {
    if (eigen != NULL)
      cvReleaseMat(&eigen);
    if (tmp != NULL)
      cvReleaseMat(&tmp);
    if (p32 != NULL)
      cvFree(&p32);
    raise_cverror(e);
  }
  VALUE corners = rb_ary_new2(np);
  for (int i = 0; i < np; ++i)
    rb_ary_store(corners, i, cCvPoint2D32f::new_object(p32[i]));
  cvFree(&p32);
  cvReleaseMat(&eigen);
  cvReleaseMat(&tmp);
  return corners;
}

/*
 * call-seq:
 *   sample_line(p1, p2[,connectivity = 8]) {|pixel| }
 *
 * not yet.
 */
VALUE
rb_sample_line(int argc, VALUE *argv, VALUE self)
{
  /*
    VALUE p1, p2, connectivity;
    if (rb_scan_args(argc, argv, "21", &p1, &p2, &connectivity) < 3)
    connectivity = INT2FIX(8);
    CvPoint point1 = VALUE_TO_CVPOINT(p1), point2 = VALUE_TO_CVPOINT(p2);
    int size;
    switch(FIX2INT(connectivity)) {
    case 4:
    size = abs(point2.x - point1.x) + abs(point2.y - point1.y) + 1;
    break;
    case 8:
    size = maxint(abs(point2.x - point1.x) + 1, abs(point2.y - point1.y) + 1);
    break;
    default:
    rb_raise(rb_eArgError, "argument 3(connectivity) should be 4 or 8. 8 is default.");
    }
    VALUE buf = cCvMat::new_object(1, size, cvGetElemType(CVARR(self)));
    cvSampleLine(CVARR(self), point1, point2, CVMAT(buf)->data.ptr, FIX2INT(connectivity));
    if (rb_block_given_p()) {
    for(int i = 0; i < size; i++) {
    //Data_Wrap_Struct(cCvScalar::rb_class(), 0, 0, CVMAT(buf)->data.ptr[]);
    //rb_yield(cCvScalar::new_object);
    }
    }
    return buf;
  */
  return Qnil;
}

/*
 * call-seq:
 *   rect_sub_pix(<i>center[, size = self.size]</i>) -> cvmat
 *
 * Retrieves pixel rectangle from image with sub-pixel accuracy.
 * Extracts pixels from <i>self</i>.
 *   dst(x,y) = self(x + center.x - (size.width - 1) * 0.5, y + center.y - (size.height - 1) * 0.5)
 * where the values of pixels at non-integer coordinates are retrived using bilinear iterpolation.
 * Every channel of multiple-channel images is processed independently.
 * Whereas the rectangle center must be inside the image, the whole rectangle may be partially occludedl.
 * In this case, the replication border mode is used to get pixel values beyond the image boundaries.
 */
VALUE
rb_rect_sub_pix(int argc, VALUE *argv, VALUE self)
{
  VALUE center, size;
  VALUE dest = Qnil;
  CvSize _size;
  CvArr* self_ptr = CVARR(self);
  try {
    if (rb_scan_args(argc, argv, "11", &center, &size) < 2)
      _size = cvGetSize(self_ptr);
    else
      _size = VALUE_TO_CVSIZE(size);
    dest = new_mat_kind_object(_size, self);
    cvGetRectSubPix(self_ptr, CVARR(dest), VALUE_TO_CVPOINT2D32F(center));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   quandrangle_sub_pix(<i>map_matrix[, size = self.size]</i>) -> cvmat
 *
 * Retrives pixel quadrangle from image with sub-pixel accuracy.
 * Extracts pixel from <i>self</i> at sub-pixel accuracy and store them:
 */
VALUE
rb_quadrangle_sub_pix(int argc, VALUE *argv, VALUE self)
{
  VALUE map_matrix, size;
  VALUE dest = Qnil;
  CvSize _size;
  CvArr* self_ptr = CVARR(self);
  try {
    if (rb_scan_args(argc, argv, "11", &map_matrix, &size) < 2)
      _size = cvGetSize(self_ptr);
    else
      _size = VALUE_TO_CVSIZE(size);
    dest = new_mat_kind_object(_size, self);
    cvGetQuadrangleSubPix(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(map_matrix));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   resize(<i>size[,interpolation = :linear]</i>) -> cvmat
 *
 * Resize image.
 * <i>interpolation</i> is interpolation method:
 * * :nn
 *   nearest-neighbor interpolation.
 * * :linear
 *   bilinear interpolation (used by default)
 * * :area
 *   resampling using pixel area relation. It is preferred method for image decimation that give moire-free results.
 *   In case of zooming it is similar to NN method.
 * * :cubic
 *   bicubic interpolation.
 * Return <i>self</i> resized image that it fits exactly to <i>size</i>. If ROI is set, the method consideres the ROI as supported as usual.
 */
VALUE
rb_resize(int argc, VALUE *argv, VALUE self)
{
  VALUE size, interpolation;
  rb_scan_args(argc, argv, "11", &size, &interpolation);
  VALUE dest = new_mat_kind_object(VALUE_TO_CVSIZE(size), self);
  try {
    cvResize(CVARR(self), CVARR(dest), CVMETHOD("INTERPOLATION_METHOD", interpolation, CV_INTER_LINEAR));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   warp_affine(<i>map_matrix[,interpolation = :linear][,option = :fill_outliers][,fillval = 0]</i>) -> cvmat
 *
 * Applies affine transformation to the image.
 */
VALUE
rb_warp_affine(int argc, VALUE *argv, VALUE self)
{
  VALUE map_matrix, interpolation, option, fill_value;
  VALUE dest = Qnil;
  if (rb_scan_args(argc, argv, "13", &map_matrix, &interpolation, &option, &fill_value) < 4)
    fill_value = INT2FIX(0);
  CvArr* self_ptr = CVARR(self);
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvWarpAffine(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(map_matrix),
		 CVMETHOD("INTERPOLATION_METHOD", interpolation, CV_INTER_LINEAR)
		 | CVMETHOD("WARP_FLAG", option, CV_WARP_FILL_OUTLIERS), VALUE_TO_CVSCALAR(fill_value));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}


/*
 * call-seq:
 *   CvMat.find_homograpy(<i>src_points, dst_points[,method = :all][,ransac_reproj_threshold = 0][,get_status = nil]</i>) -> cvmat
 * 
 * Finds the perspective transformation between two planes.
 * <i>src_points:</i> Coordinates of the points in the original plane, 2xN, Nx2, 3xN or Nx3 1-channel array (the latter two are for representation in homogeneous coordinates), where N is the number of points. 1xN or Nx1 2- or 3-channel array can also be passed.
 * <i>dst_points:</i> Point coordinates in the destination plane, 2xN, Nx2, 3xN or Nx3 1-channel, or 1xN or Nx1 2- or 3-channel array.
 * <i>method:</i> The method used to computed homography matrix; one of the following symbols:
 *     :all - a regular method using all the points
 *     :ransac - RANSAC-based robust method
 *     :lmeds - Least-Median robust method
 * <i>ransac_reproj_threshold:</i> The maximum allowed reprojection error to treat a point pair as an inlier (used in the RANSAC method only). If src_points and dst_points are measured in pixels, it usually makes sense to set this parameter somewhere in the range 1 to 10.
 * <i>get_status</i> If true, the optional output mask set by a robust method (:ransac or :lmeds) is returned additionally.
 */
VALUE
rb_find_homograpy(int argc, VALUE *argv, VALUE self)
{
  VALUE src_points, dst_points, method, ransac_reproj_threshold, get_status;
  rb_scan_args(argc, argv, "23", &src_points, &dst_points, &method, &ransac_reproj_threshold, &get_status);

  VALUE homography = new_object(cvSize(3, 3), CV_32FC1);
  int _method = CVMETHOD("HOMOGRAPHY_CALC_METHOD", method, 0);
  double _ransac_reproj_threshold = NIL_P(ransac_reproj_threshold) ? 0.0 : NUM2DBL(ransac_reproj_threshold);

  if ((_method != 0) && (!NIL_P(get_status)) && IF_BOOL(get_status, 1, 0, 0)) {
    CvMat *src = CVMAT_WITH_CHECK(src_points);
    int num_points = MAX(src->rows, src->cols);
    VALUE status = new_object(cvSize(num_points, 1), CV_8UC1);
    try {
      cvFindHomography(src, CVMAT_WITH_CHECK(dst_points), CVMAT(homography),
		       _method, _ransac_reproj_threshold, CVMAT(status));
    }
    catch (cv::Exception& e) {
      raise_cverror(e);
    }
    return rb_assoc_new(homography, status);
  }
  else {
    try {
      cvFindHomography(CVMAT(src_points), CVMAT(dst_points), CVMAT(homography),
		       _method, _ransac_reproj_threshold, NULL);
    }
    catch (cv::Exception& e) {
      raise_cverror(e);
    }
    return homography;
  }
}

/*
 * call-seq:
 *   CvMat.rotation_matrix2D(<i>center,angle,scale</i>) -> cvmat
 *
 * Create new affine matrix of 2D rotation (2x3 32bit floating-point matrix).
 * <i>center</i> is center of rotation (x, y).
 * <i>angle</i> is rotation angle in degrees.
 * Positive values mean counter-clockwise rotation
 * (the coordinate origin is assumed at top-left corner).
 * <i>scale</i> is isotropic scale factor.
 *
 *  [ a b | (1 - a) * center.x - b * center.y  ]
 *  [-b a | (b * center.x + (1 + a) * center.y ]
 *  where a = scale * cos(angle), b = scale * sin(angle)
 */
VALUE
rb_rotation_matrix2D(VALUE self, VALUE center, VALUE angle, VALUE scale)
{
  VALUE map_matrix = new_object(cvSize(3, 2), CV_MAKETYPE(CV_32F, 1));
  try {
    cv2DRotationMatrix(VALUE_TO_CVPOINT2D32F(center), NUM2DBL(angle), NUM2DBL(scale), CVMAT(map_matrix));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return map_matrix;
}

/*
 * call-seq:
 *   warp_perspective(<i>map_matrix[,interpolation=:linear][,option =:fill_outliers][,fillval=0])</i>) -> cvmat
 *
 * Applies perspective transformation to the image.
 */
VALUE
rb_warp_perspective(int argc, VALUE *argv, VALUE self)
{
  VALUE map_matrix, interpolation, option, fillval;
  if (rb_scan_args(argc, argv, "13", &map_matrix, &interpolation, &option, &fillval) < 4)
    fillval = INT2FIX(0);
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvWarpPerspective(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(map_matrix),
		      CVMETHOD("INTERPOLATION_METHOD", interpolation, CV_INTER_LINEAR)
		      | CVMETHOD("WARP_FLAG",option, CV_WARP_FILL_OUTLIERS),
		      VALUE_TO_CVSCALAR(fillval));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   remap(<i>mapx,mapy[,interpolation=:linear][,option=:fill_outliers][,fillval=0]</i>) -> cvmat
 *
 * Applies generic geometrical transformation to the image.
 * Transforms source image using the specified map:
 *   dst(x,y)<-src(mapx(x,y),mapy(x,y))
 * Similar to other geometrical transformations, some interpolation method (specified by user) is used to
 * extract pixels with non-integer coordinates.
 */
VALUE
rb_remap(int argc, VALUE *argv, VALUE self)
{
  VALUE mapx, mapy, interpolation, option, fillval;
  if (rb_scan_args(argc, argv, "23", &mapx, &mapy, &interpolation, &option, &fillval) < 5)
    fillval = INT2FIX(0);
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvRemap(self_ptr, CVARR(dest), CVARR_WITH_CHECK(mapx), CVARR_WITH_CHECK(mapy),
	    CVMETHOD("INTERPOLATION_METHOD", interpolation, CV_INTER_LINEAR)
	    | CVMETHOD("WARP_FLAG", option, CV_WARP_FILL_OUTLIERS),
	    VALUE_TO_CVSCALAR(fillval));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   log_polar(<i>size, center, magnitude[ ,flags=CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS]</i>)
 *
 * Remaps image to log-polar space.
 */
VALUE
rb_log_polar(int argc, VALUE *argv, VALUE self)
{
  VALUE dst_size, center, m, flags;
  rb_scan_args(argc, argv, "31", &dst_size, &center, &m, &flags);
  int _flags = NIL_P(flags) ? (CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS) : NUM2INT(flags);
  VALUE dest = new_mat_kind_object(VALUE_TO_CVSIZE(dst_size), self);
  try {
    cvLogPolar(CVARR(self), CVARR(dest), VALUE_TO_CVPOINT2D32F(center), NUM2DBL(m), _flags);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   erode(<i>[element = nil, iteration = 1]</i>) -> cvmat
 *
 * Create erodes image by using arbitrary structuring element.
 * <i>element</i> is structuring element used for erosion.
 * <i>element</i> should be IplConvKernel. If it is nil, a 3x3 rectangular structuring element is used.
 * <i>iterations</i> is number of times erosion is applied.
 */
VALUE
rb_erode(int argc, VALUE *argv, VALUE self)
{
  return rb_erode_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   erode!(<i>[element = nil][,iteration = 1]</i>) -> self
 *
 * Erodes image by using arbitrary structuring element.
 * see also #erode.
 */
VALUE
rb_erode_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE element, iteration;
  rb_scan_args(argc, argv, "02", &element, &iteration);
  IplConvKernel* kernel = NIL_P(element) ? NULL : IPLCONVKERNEL_WITH_CHECK(element);
  try {
    cvErode(CVARR(self), CVARR(self), kernel, IF_INT(iteration, 1));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   dilate(<i>[element = nil][,iteration = 1]</i>) -> cvmat
 *
 * Create dilates image by using arbitrary structuring element.
 * <i>element</i> is structuring element used for erosion.
 * <i>element</i> should be IplConvKernel. If it is nil, a 3x3 rectangular structuring element is used.
 * <i>iterations</i> is number of times erosion is applied.
 */
VALUE
rb_dilate(int argc, VALUE *argv, VALUE self)
{
  return rb_dilate_bang(argc, argv, rb_clone(self));
}

/*
 * call-seq:
 *   dilate!(<i>[element = nil][,iteration = 1]</i>) -> self
 *
 * Dilate image by using arbitrary structuring element.
 * see also #dilate.
 */
VALUE
rb_dilate_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE element, iteration;
  rb_scan_args(argc, argv, "02", &element, &iteration);
  IplConvKernel* kernel = NIL_P(element) ? NULL : IPLCONVKERNEL_WITH_CHECK(element);
  try {
    cvDilate(CVARR(self), CVARR(self), kernel, IF_INT(iteration, 1));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

VALUE
rb_morphology_internal(VALUE element, VALUE iteration, int operation, VALUE self)
{
  CvArr* self_ptr = CVARR(self);
  CvSize size = cvGetSize(self_ptr);
  VALUE dest = new_mat_kind_object(size, self);
  IplConvKernel* kernel = NIL_P(element) ? NULL : IPLCONVKERNEL_WITH_CHECK(element);
  try {
    if (operation == CV_MOP_GRADIENT) {
      CvMat* temp = rb_cvCreateMat(size.height, size.width, cvGetElemType(self_ptr));
      cvMorphologyEx(self_ptr, CVARR(dest), temp, kernel, CV_MOP_GRADIENT, IF_INT(iteration, 1));
      cvReleaseMat(&temp);
    }
    else {
      cvMorphologyEx(self_ptr, CVARR(dest), 0, kernel, operation, IF_INT(iteration, 1));
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   morpholohy(<i>operation[,element = nil][,iteration = 1]</i>) -> cvmat
 *
 * Performs advanced morphological transformations.
 * <i>operation</i>
 * Type of morphological operation, one of:
 *   CV_MOP_OPEN - opening
 *   CV_MOP_CLOSE - closing
 *   CV_MOP_GRADIENT - morphological gradient
 *   CV_MOP_TOPHAT - top hat
 *   CV_MOP_BLACKHAT - black hat
 */
VALUE
rb_morphology(int argc, VALUE *argv, VALUE self)
{
  VALUE element, iteration, operation;
  rb_scan_args(argc, argv, "12", &operation, &element, &iteration);
  return rb_morphology_internal(element, iteration, CVMETHOD("MORPHOLOGICAL_OPERATION", operation, -1), self);
}

/*
 * call-seq:
 *   smooth_blur_no_scale(<i>[p1 = 3, p2 = 3]</i>) -> cvmat
 *
 * Smooths the image by simple blur with no scaling.
 * * 8bit unsigned -> return 16bit unsigned
 * * 32bit floating point -> return 32bit floating point
 * <b>support single-channel image only.</b>
 */
VALUE
rb_smooth_blur_no_scale(int argc, VALUE *argv, VALUE self)
{
  VALUE p1, p2, dest;
  rb_scan_args(argc, argv, "02", &p1, &p2);
  CvArr* self_ptr = CVARR(self);
  int type = cvGetElemType(self_ptr), dest_type;
  switch (CV_MAT_DEPTH(type)) {
  case CV_8U:
    dest_type = CV_16U;
    break;
  case CV_32F:
    dest_type = CV_32F;
    break;
  default:
    rb_raise(rb_eNotImpError, "unsupport format. (support 8bit unsigned/signed or 32bit floating point only)");
  }
  dest = new_mat_kind_object(cvGetSize(self_ptr), self, dest_type, CV_MAT_CN(type));
  cvSmooth(self_ptr, CVARR(dest), CV_BLUR_NO_SCALE, IF_INT(p1, 3), IF_INT(p2, 3));
  return dest;
}

/*
 * call-seq:
 *   smooth_blur(<i>[p1 = 3, p2 = 3]</i>) -> cvmat
 *
 * Smooths the image by simple blur.
 * Summation over a pixel <i>p1</i> x <i>p2</i> neighborhood with subsequent scaling by 1 / (p1*p2).
 */
VALUE
rb_smooth_blur(int argc, VALUE *argv, VALUE self)
{
  VALUE p1, p2, dest;
  rb_scan_args(argc, argv, "02", &p1, &p2);
  CvArr* self_ptr = CVARR(self);
  dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  cvSmooth(self_ptr, CVARR(dest), CV_BLUR, IF_INT(p1, 3), IF_INT(p2, 3));
  return dest;
}

/*
 * call-seq:
 *   smooth_gaussian(<i>[p1 = 3, p2 = 3, p3 = 0.0, p4 = 0.0]</i>) -> cvmat
 *
 * Smooths the image by gaussian blur.
 * Convolving image with <i>p1</i> x <i>p2</i> Gaussian kernel.
 *
 * <i>p3</i> may specify Gaussian sigma (standard deviation).
 * If it is zero, it is calculated from the kernel size:
 *    sigma = (n/2 - 1)*0.3 + 0.8, where n = p1 for horizontal kernel,
 *                                       n = p2 for vertical kernel.
 *
 * <i>p4</i> is in case of non-square Gaussian kernel the parameter.
 * It may be used to specify a different (from p3) sigma in the vertical direction.
 */
VALUE
rb_smooth_gaussian(int argc, VALUE *argv, VALUE self)
{
  VALUE p1, p2, p3, p4, dest;
  rb_scan_args(argc, argv, "04", &p1, &p2, &p3, &p4);
  CvArr* self_ptr = CVARR(self);
  dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  cvSmooth(self_ptr, CVARR(dest), CV_GAUSSIAN, IF_INT(p1, 3), IF_INT(p2, 3), IF_DBL(p3, 0.0), IF_DBL(p4, 0.0));
  return dest;
}

/*
 * call-seq:
 *   smooth_median(<i>[p1 = 3]</i>) -> cvmat
 *
 * Smooths the image by median blur.
 * Finding median of <i>p1</i> x <i>p1</i> neighborhood (i.e. the neighborhood is square).
 */
VALUE
rb_smooth_median(int argc, VALUE *argv, VALUE self)
{
  VALUE p1, dest;
  rb_scan_args(argc, argv, "01", &p1);
  CvArr* self_ptr = CVARR(self);
  dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  cvSmooth(self_ptr, CVARR(dest), CV_MEDIAN, IF_INT(p1, 3));
  return dest;
}

/*
 * call-seq:
 *   smooth_bilateral(<i>[p1 = 3][p2 = 3]</i>) -> cvmat
 *
 * Smooths the image by bilateral filter.
 * Applying bilateral 3x3 filtering with color sigma=<i>p1</i> and space sigma=<i>p2</i>.
 */
VALUE
rb_smooth_bilateral(int argc, VALUE *argv, VALUE self)
{
  VALUE p1, p2, dest;
  rb_scan_args(argc, argv, "02", &p1, &p2);
  CvArr* self_ptr = CVARR(self);
  dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  cvSmooth(self_ptr, CVARR(dest), CV_BILATERAL, IF_INT(p1, 3), IF_INT(p2, 3));
  return dest;
}

VALUE
rb_smooth(int argc, VALUE *argv, VALUE self)
{
  VALUE smoothtype, p1, p2, p3, p4;
  rb_scan_args(argc, argv, "14", &smoothtype, &p1, &p2, &p3, &p4);
  int _smoothtype = CVMETHOD("SMOOTHING_TYPE", smoothtype, -1);
  
  VALUE (*smooth_func)(int c, VALUE* v, VALUE s);
  argc--;
  switch (_smoothtype) {
  case CV_BLUR_NO_SCALE:
    smooth_func = rb_smooth_blur_no_scale;
    argc = (argc > 2) ? 2 : argc;
    break;
  case CV_BLUR:
    smooth_func = rb_smooth_blur;
    argc = (argc > 2) ? 2 : argc;
    break;
  case CV_GAUSSIAN:
    smooth_func = rb_smooth_gaussian;
    break;
  case CV_MEDIAN:
    smooth_func = rb_smooth_median;
    argc = (argc > 1) ? 1 : argc;
    break;
  case CV_BILATERAL:
    smooth_func = rb_smooth_bilateral;
    argc = (argc > 2) ? 2 : argc;
    break;
  default:
    smooth_func = rb_smooth_gaussian;
    break;
  }
  VALUE result = Qnil;
  try {
    result = (*smooth_func)(argc, argv + 1, self);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return result;
}

/*
 * call-seq:
 *   filter2d(<i>kernel[,anchor]</i>) -> cvmat
 *
 * Convolves image with the kernel.
 * Convolution kernel, single-channel floating point matrix (or same depth of self's).
 * If you want to apply different kernels to different channels,
 * split the image using CvMat#split into separate color planes and process them individually.
 */
VALUE
rb_filter2d(int argc, VALUE *argv, VALUE self)
{
  VALUE _kernel, _anchor;
  rb_scan_args(argc, argv, "11", &_kernel, &_anchor);
  CvMat* kernel = CVMAT_WITH_CHECK(_kernel);
  CvArr* self_ptr = CVARR(self);
  VALUE _dest = Qnil;
  try {
    int type = cvGetElemType(kernel);
    _dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvFilter2D(self_ptr, CVARR(_dest), kernel, NIL_P(_anchor) ? cvPoint(-1,-1) : VALUE_TO_CVPOINT(_anchor));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return _dest;
}

/*
 * call-seq:
 *   copy_make_border(<i>border_type, size, offset[,value = CvScalar.new(0)]</i>)
 *
 * Copies image and makes border around it.
 * <i>border_type</i>:
 * - IPL_BORDER_CONSTANT, :constant
 *     border is filled with the fixed value, passed as last parameter of the function.
 * - IPL_BORDER_REPLICATE, :replicate
 *     the pixels from the top and bottom rows, the left-most and right-most columns are replicated to fill the border
 * <i>size</i>: The destination image size
 * <i>offset</i>: Coordinates of the top-left corner (or bottom-left in the case of images with bottom-left origin) of the destination image rectangle.
 * <i>value</i>: Value of the border pixels if bordertype is IPL_BORDER_CONSTANT or :constant.
 */
VALUE
rb_copy_make_border(int argc, VALUE *argv, VALUE self)
{
  VALUE border_type, size, offset, value, dest;
  rb_scan_args(argc, argv, "31", &border_type, &size, &offset, &value);
  dest = new_mat_kind_object(VALUE_TO_CVSIZE(size), self);

  int type = 0;
  if (SYMBOL_P(border_type)) {
    ID type_id = rb_to_id(border_type);
    if (type_id == rb_intern("constant"))
      type = IPL_BORDER_CONSTANT;
    else if (type_id == rb_intern("replicate"))
      type = IPL_BORDER_REPLICATE;
    else
      rb_raise(rb_eArgError, "Invalid border_type (should be :constant or :replicate)");
  }
  else
    type = NUM2INT(border_type);

  try {
    cvCopyMakeBorder(CVARR(self), CVARR(dest), VALUE_TO_CVPOINT(offset), type,
		     NIL_P(value) ? cvScalar(0) : VALUE_TO_CVSCALAR(value));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   integral(<i>need_sqsum = false, need_tilted_sum = false</i>) -> [cvmat, cvmat or nil, cvmat or nil]
 *
 * Calculates integral images.
 * If <i>need_sqsum</i> = true, calculate the integral image for squared pixel values.
 * If <i>need_tilted_sum</i> = true, calculate the integral for the image rotated by 45 degrees.
 *
 *   sum(X,Y)=sumx<X,y<Yimage(x,y)
 *   sqsum(X,Y)=sumx<X,y<Yimage(x,y)2
 *   tilted_sum(X,Y)=sumy<Y,abs(x-X)<yimage(x,y)
 *
 * Using these integral images, one may calculate sum, mean, standard deviation over arbitrary up-right or rotated rectangular region of the image in a constant time.
 */
VALUE
rb_integral(int argc, VALUE *argv, VALUE self)
{
  VALUE need_sqsum = Qfalse, need_tiled_sum = Qfalse;
  rb_scan_args(argc, argv, "02", &need_sqsum, &need_tiled_sum);

  VALUE sum = Qnil;
  VALUE sqsum = Qnil;
  VALUE tiled_sum = Qnil;
  CvArr* self_ptr = CVARR(self);
  try {
    CvSize self_size = cvGetSize(self_ptr);
    CvSize size = cvSize(self_size.width + 1, self_size.height + 1);
    int type_cv64fcn = CV_MAKETYPE(CV_64F, CV_MAT_CN(cvGetElemType(self_ptr)));
    sum = cCvMat::new_object(size, type_cv64fcn);
    sqsum = (need_sqsum == Qtrue ? cCvMat::new_object(size, type_cv64fcn) : Qnil);
    tiled_sum = (need_tiled_sum == Qtrue ? cCvMat::new_object(size, type_cv64fcn) : Qnil);
    cvIntegral(self_ptr, CVARR(sum), (need_sqsum == Qtrue) ? CVARR(sqsum) : NULL,
	       (need_tiled_sum == Qtrue) ? CVARR(tiled_sum) : NULL);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  
  if ((need_sqsum != Qtrue) && (need_tiled_sum != Qtrue))
    return sum;
  else {
    VALUE dest = rb_ary_new3(1, sum);
    if (need_sqsum == Qtrue)
      rb_ary_push(dest, sqsum);
    if (need_tiled_sum == Qtrue)
      rb_ary_push(dest, tiled_sum);
    return dest;
  }
}

inline VALUE
rb_threshold_internal(int threshold_type, VALUE threshold, VALUE max_value, VALUE use_otsu, VALUE self)
{
  CvArr* self_ptr = CVARR(self);
  int otsu = (use_otsu == Qtrue) && ((threshold_type & CV_THRESH_OTSU) == 0);
  int type = threshold_type | (otsu ? CV_THRESH_OTSU : 0);
  VALUE dest = Qnil;
  double otsu_threshold = 0;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    otsu_threshold = cvThreshold(self_ptr, CVARR(dest), NUM2DBL(threshold), NUM2DBL(max_value), type);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  if ((use_otsu == Qtrue) || (threshold_type & CV_THRESH_OTSU))
    return rb_assoc_new(dest, DBL2NUM(otsu_threshold));
  else
    return dest;
}

/*
 * call-seq:
 *   threshold(<i>threshold, max_value, threshold_type[,use_otsu = false]</i>)
 *
 * Applies fixed-level threshold to array elements.
 *
 */
VALUE
rb_threshold(int argc, VALUE *argv, VALUE self)
{
  VALUE threshold, max_value, threshold_type, use_otsu;
  rb_scan_args(argc, argv, "31", &threshold, &max_value, &threshold_type, &use_otsu);
  const int INVALID_TYPE = -1;
  int type = CVMETHOD("THRESHOLD_TYPE", threshold_type, INVALID_TYPE);
  if (type == INVALID_TYPE)
    rb_raise(rb_eArgError, "Invalid threshold type.");
  
  return rb_threshold_internal(type, threshold, max_value, use_otsu, self);
}


/*
 * call-seq:
 *   adaptive_threshold(max_value[, options]) -> cvmat
 *
 * Applies an adaptive threshold to an array.
 *
 * ==== params:
 *
 * * <b>max_value (Number)</b> - Maximum value that is used with +CV_THRESH_BINARY+ and +CV_THRESH_BINARY_INV+
 * * <b>options (Hash)</b> - threshold option
 *   * <b>:threshold_type (Integer or Symbol)</b> - Thresholding type; must be one of +CV_THRESH_BINARY+ or +:binary+, +CV_THRESH_BINARY_INV+ or +:binary_inv+ (default: +CV_THRESH_BINARY+)
 *   * <b>:adaptive_method (Integer or Symbol)</b> - Adaptive thresholding algorithm to use: +CV_ADAPTIVE_THRESH_MEAN_C+ or +:mean_c+, +CV_ADAPTIVE_THRESH_GAUSSIAN_C+ or +:gaussian_c+ (default: +CV_ADAPTIVE_THRESH_MEAN_C+)
 *   * <b>:block_size (Integer)</b> - The size of a pixel neighborhood that is used to calculate a threshold value for the pixel: 3, 5, 7, and so on (default: 3)
 *   * <b>:param1 (Number)</b> - The method-dependent parameter. For the methods +CV_ADAPTIVE_THRESH_MEAN_C+ and +CV_ADAPTIVE_THRESH_GAUSSIAN_C+ it is a constant subtracted from the mean or weighted mean, though it may be negative (default: 5)
 */
VALUE
rb_adaptive_threshold(int argc, VALUE *argv, VALUE self)
{
  VALUE max_value, options;
  rb_scan_args(argc, argv, "11", &max_value, &options);

  int threshold_type = CV_THRESH_BINARY;
  int adaptive_method = CV_ADAPTIVE_THRESH_MEAN_C;
  int block_size = 3;
  double param1 = 5;
  if (!NIL_P(options)) {
    Check_Type(options, T_HASH);
    threshold_type = CVMETHOD("THRESHOLD_TYPE", LOOKUP_CVMETHOD(options, "threshold_type"),
			      CV_THRESH_BINARY);
    adaptive_method = CVMETHOD("ADAPTIVE_METHOD", LOOKUP_CVMETHOD(options, "adaptive_method"),
			       CV_ADAPTIVE_THRESH_MEAN_C);
    VALUE _block_size = LOOKUP_CVMETHOD(options, "block_size");
    if (!NIL_P(_block_size)) {
      block_size = NUM2INT(_block_size);
    }
    VALUE _param1 = LOOKUP_CVMETHOD(options, "param1");
    if (!NIL_P(_param1)) {
      param1 = NUM2INT(_param1);
    }
  }
  CvArr* self_ptr = CVARR(self);
  VALUE dst = new_mat_kind_object(cvGetSize(self_ptr), self);
  try {
    cvAdaptiveThreshold(self_ptr, CVARR(dst), NUM2DBL(max_value), adaptive_method, threshold_type,
			block_size, param1);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  
  return dst;
}

/*
 * call-seq:
 *   pyr_down(<i>[filter = :gaussian_5x5]</i>) -> cvmat
 *
 * Return downsamples image.
 *
 * This operation performs downsampling step of Gaussian pyramid decomposition.
 * First it convolves source image with the specified filter and then downsamples the image by rejecting even rows and columns.
 *
 * note: filter - only :gaussian_5x5 is currently supported.
 */
VALUE
rb_pyr_down(int argc, VALUE *argv, VALUE self)
{
  int filter = CV_GAUSSIAN_5x5;
  if (argc > 0) {
    VALUE filter_type = argv[0];
    switch (TYPE(filter_type)) {
    case T_SYMBOL:
      // currently suport CV_GAUSSIAN_5x5 only.
      break;
    default:
      raise_typeerror(filter_type, rb_cSymbol);
    }
  }
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    CvSize original_size = cvGetSize(self_ptr);
    CvSize size = { original_size.width >> 1, original_size.height >> 1 };
    dest = new_mat_kind_object(size, self);
    cvPyrDown(self_ptr, CVARR(dest), filter);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   pyr_up(<i>[filter = :gaussian_5x5]</i>) -> cvmat
 *
 * Return upsamples image.
 *
 * This operation performs up-sampling step of Gaussian pyramid decomposition.
 * First it upsamples the source image by injecting even zero rows and columns and then convolves result with the specified filter multiplied by 4 for interpolation.
 * So the destination image is four times larger than the source image.
 *
 * note: filter - only :gaussian_5x5 is currently supported.
 */
VALUE
rb_pyr_up(int argc, VALUE *argv, VALUE self)
{
  VALUE filter_type;
  rb_scan_args(argc, argv, "01", &filter_type);
  int filter = CV_GAUSSIAN_5x5;
  if (argc > 0) {
    switch (TYPE(filter_type)) {
    case T_SYMBOL:
      // currently suport CV_GAUSSIAN_5x5 only.
      break;
    default:
      raise_typeerror(filter_type, rb_cSymbol);
    }
  }
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    CvSize original_size = cvGetSize(self_ptr);
    CvSize size = { original_size.width << 1, original_size.height << 1 };
    dest = new_mat_kind_object(size, self);
    cvPyrUp(self_ptr, CVARR(dest), filter);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   flood_fill(<i>seed_point, new_val, lo_diff, up_diff[,flood_fill_option]</i>) -> [cvmat, cvconnectedcomp, iplimage(mask)]
 *
 * Return image filled a connnected compoment with given color.
 * This operation fills a connected component starting from the seed point with the specified color.
 * The connectivity is determined by the closeness of pixel values.
 * The pixel at (x, y) is considered to belong to the repainted domain if:
 *
 *   src(x',y')-lo_diff<=src(x,y)<=src(x',y')+up_diff,     grayscale image, floating range
 *   src(seed.x,seed.y)-lo<=src(x,y)<=src(seed.x,seed.y)+up_diff, grayscale image, fixed range
 *   src(x',y')r-lo_diffr<=src(x,y)r<=src(x',y')r+up_diffr and
 *   src(x',y')g-lo_diffg<=src(x,y)g<=src(x',y')g+up_diffg and
 *   src(x',y')b-lo_diffb<=src(x,y)b<=src(x',y')b+up_diffb, color image, floating range
 *   src(seed.x,seed.y)r-lo_diffr<=src(x,y)r<=src(seed.x,seed.y)r+up_diffr and
 *   src(seed.x,seed.y)g-lo_diffg<=src(x,y)g<=src(seed.x,seed.y)g+up_diffg and
 *   src(seed.x,seed.y)b-lo_diffb<=src(x,y)b<=src(seed.x,seed.y)b+up_diffb, color image, fixed range
 *
 * where src(x',y') is value of one of pixel neighbors.
 * That is, to be added to the connected component, a pixel's color/brightness should be close enough to:
 * * color/brightness of one of its neighbors that are already referred to the connected component in case of floating range
 * * color/brightness of the seed point in case of fixed range.
 *
 * arguments
 * * seed_point -The starting point.
 * * new_val - New value of repainted domain pixels.
 * * lo_diff - Maximal lower brightness/color difference between the currently observed pixel and one of its neighbor belong to the component or seed pixel to add the pixel to component. In case of 8-bit color images it is packed value.
 * * up_diff - Maximal upper brightness/color difference between the currently observed pixel and one of its neighbor belong to the component or seed pixel to add the pixel to component. In case of 8-bit color images it is packed value.
 *
 * and flood_fill_option
 *   :connectivity => 4 or 8, 4 default
 *     Connectivity determines which neighbors of a pixel are considered.
 *   :fixed_range => true or false, false default
 *     If set the difference between the current pixel and seed pixel is considered, otherwise difference between neighbor pixels is considered (the range is floating).
 *   :mask_only => true or false, false default
 *     If set, the function does not fill the image(new_val is ignored), but the fills mask.
 *
 * note: <i>flood_fill_option</i>'s default value is CvMat::FLOOD_FILL_OPTION.
 */
VALUE
rb_flood_fill(int argc, VALUE *argv, VALUE self)
{
  return rb_flood_fill_bang(argc, argv, copy(self));
}

/*
 * call-seq:
 *   flood_fill!(<i>seed_point, new_val, lo_diff, up_diff[,flood_fill_option]</i>) -> [self, cvconnectedcomp, iplimage(mask)]
 *
 * Fills a connected component with given color.
 * see CvMat#flood_fill
 */
VALUE
rb_flood_fill_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE seed_point, new_val, lo_diff, up_diff, flood_fill_option;
  rb_scan_args(argc, argv, "23", &seed_point, &new_val, &lo_diff, &up_diff, &flood_fill_option);
  flood_fill_option = FLOOD_FILL_OPTION(flood_fill_option);
  int flags = FF_CONNECTIVITY(flood_fill_option);
  if (FF_FIXED_RANGE(flood_fill_option)) {
    flags |= CV_FLOODFILL_FIXED_RANGE;
  }
  if (FF_MASK_ONLY(flood_fill_option)) {
    flags |= CV_FLOODFILL_MASK_ONLY;
  }
  CvArr* self_ptr = CVARR(self);
  VALUE comp = cCvConnectedComp::new_object();
  VALUE mask = Qnil;
  try {
    CvSize size = cvGetSize(self_ptr);
    // TODO: Change argument format to set mask
    mask = new_object(size.width + 2, size.height + 2, CV_MAKETYPE(CV_8U, 1));
    CvMat* mask_ptr = CVMAT(mask);
    cvSetZero(mask_ptr);
    cvFloodFill(self_ptr,
		VALUE_TO_CVPOINT(seed_point),
		VALUE_TO_CVSCALAR(new_val),
		NIL_P(lo_diff) ? cvScalar(0) : VALUE_TO_CVSCALAR(lo_diff),
		NIL_P(up_diff) ? cvScalar(0) : VALUE_TO_CVSCALAR(up_diff),
		CVCONNECTEDCOMP(comp),
		flags,
		mask_ptr);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(3, self, comp, mask);
}

/*
 * call-seq:
 *   find_contours([find_contours_options]) -> cvchain or cvcontour or nil
 *
 * Finds contours in binary image, and return contours as CvContour or CvChain.
 * If contours not found, return nil.
 *
 * <i>flood_fill_option</i> should be Hash include these keys.
 *   :mode - Retrieval mode.
 *      :external - retrive only the extreme outer contours
 *      :list - retrieve all the contours and puts them in the list.(default)
 *      :ccomp - retrieve all the contours and organizes them into two-level hierarchy:
 *               top level are external boundaries of the components, second level are bounda boundaries of the holes
 *      :tree - retrieve all the contours and reconstructs the full hierarchy of nested contours
 *              Connectivity determines which neighbors of a pixel are considered.
 *   :method - Approximation method.
 *      :code - output contours in the Freeman chain code. All other methods output polygons (sequences of vertices).
 *      :approx_none - translate all the points from the chain code into points;
 *      :approx_simple - compress horizontal, vertical, and diagonal segments, that is, the function leaves only their ending points;(default)
 *      :approx_tc89_l1
 *      :approx_tc89_kcos - apply one of the flavors of Teh-Chin chain approximation algorithm.
 *      If set the difference between the current pixel and seed pixel is considered,
 *      otherwise difference between neighbor pixels is considered (the range is floating).
 *   :offset - Offset, by which every contour point is shifted.
 *             This is useful if the contours are extracted from the image ROI
 *             and then they should be analyzed in the whole image context. Should be CvPoint.
 *
 * note: <i>find_contours_option</i>'s default value is CvMat::FIND_CONTOURS_OPTION.
 *
 * <b>support single-channel 8bit unsigned image only.</b>
 *
 * note: Non-zero pixels are treated as 1's, zero pixels remain 0's
 * that is image treated as binary. To get such a binary image from grayscale,
 * one may use threshold, adaptive_threshold or canny.
 */
VALUE
rb_find_contours(int argc, VALUE *argv, VALUE self)
{
  return rb_find_contours_bang(argc, argv, copy(self));
}

/*
 * call-seq:
 *   find_contours!([find_contours_options]) -> cvchain or chcontour or nil
 *
 * Finds contours in binary image.
 * The function modifies the source image content.
 * (Because the copy is not made, it is slightly faster than find_contours.)
 *
 * see find_contours
 *
 * <b>support single-channel 8bit unsigned image only.</b>
 */
VALUE
rb_find_contours_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE find_contours_option, klass, element_klass, storage;
  rb_scan_args(argc, argv, "01", &find_contours_option);
  CvSeq *contour = NULL;
  find_contours_option = FIND_CONTOURS_OPTION(find_contours_option);
  int mode = FC_MODE(find_contours_option);
  int method = FC_METHOD(find_contours_option);
  int header_size;
  if (method == CV_CHAIN_CODE) {
    klass = cCvChain::rb_class();
    element_klass = T_FIXNUM;
    header_size = sizeof(CvChain);
  }
  else {
    klass = cCvContour::rb_class();
    element_klass = cCvPoint::rb_class();
    header_size = sizeof(CvContour);
  }
  storage = cCvMemStorage::new_object();

  int count = 0;
  try {
    count = cvFindContours(CVARR(self), CVMEMSTORAGE(storage), &contour, header_size,
			   mode, method, FC_OFFSET(find_contours_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  if (count == 0)
    return Qnil;
  else
    return cCvSeq::new_sequence(klass, contour, element_klass, storage);
}

/*
 * call-seq:
 *   draw_contours(contour, external_color, hole_color, max_level, options) -> cvmat
 *
 * Draws contour outlines or interiors in an image.
 *
 * * <i>contour</i> (CvContour) - Pointer to the first contour
 * * <i>external_color</i> (CvScalar) -  Color of the external contours
 * * <i>hole_color</i> (CvScalar) - Color of internal contours (holes)
 * * <i>max_level</i> (Integer) - Maximal level for drawn contours. If 0, only contour is drawn. If 1, the contour and all contours following it on the same level are drawn. If 2, all contours following and all contours one level below the contours are drawn, and so forth. If the value is negative, the function does not draw the contours following after contour but draws the child contours of contour up to the |max_level| - 1 level.
 * * <i>options</i> (Hash) - Drawing options.
 *   * :thickness (Integer) - Thickness of lines the contours are drawn with. If it is negative, the contour interiors are drawn (default: 1).
 *   * :line_type (Integer or Symbol) - Type of the contour segments, see CvMat#line description (default: 8).
 */
VALUE
rb_draw_contours(int argc, VALUE *argv, VALUE self)
{
  return rb_draw_contours_bang(argc, argv, copy(self));
}

/*
 * call-seq:
 *   draw_contours!(contour, external_color, hole_color, max_level, options) -> cvmat
 *
 * Draws contour outlines or interiors in an image.
 *
 * see CvMat#draw_contours
 */
VALUE
rb_draw_contours_bang(int argc, VALUE *argv, VALUE self)
{
  VALUE contour, external_color, hole_color, max_level, options;
  rb_scan_args(argc, argv, "41", &contour, &external_color, &hole_color, &max_level, &options);
  options = DRAWING_OPTION(options);
  try {
    cvDrawContours(CVARR(self), CVSEQ_WITH_CHECK(contour), VALUE_TO_CVSCALAR(external_color),
		   VALUE_TO_CVSCALAR(hole_color), NUM2INT(max_level),
		   DO_THICKNESS(options), DO_LINE_TYPE(options));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return self;
}

/*
 * call-seq:
 *   pyr_segmentation(<i>level, threshold1, threshold2</i>) -> [cvmat, cvseq(include cvconnectedcomp)]
 *
 * Does image segmentation by pyramids.
 * The pyramid builds up to the level <i>level<i>.
 * The links between any pixel a on <i>level<i>i and
 * its candidate father pixel b on the adjacent level are established if
 *   p(c(a),c(b)) < threshold1. After the connected components are defined, they are joined into several clusters. Any two segments A and B belong to the same cluster, if
 *   p(c(A),c(B)) < threshold2. The input image has only one channel, then
 *   p(c^2,c^2)=|c^2-c^2|. If the input image has three channels (red, green and blue), then
 *   p(c^2,c^2)=0,3*(c^2 r-c^2 r)+0.59*(c^2 g-c^2 g)+0,11*(c^2 b-c^2 b) . There may be more than one connected component per a cluster.
 *
 * Return segmented image and sequence of connected components.
 * <b>support single-channel or 3-channel 8bit unsigned image only</b>
 */
VALUE
rb_pyr_segmentation(VALUE self, VALUE level, VALUE threshold1, VALUE threshold2)
{
  IplImage* self_ptr = IPLIMAGE(self);
  CvSeq *comp = NULL;
  VALUE storage = cCvMemStorage::new_object();
  VALUE dest = Qnil;
  try {
    dest = cIplImage::new_object(cvGetSize(self_ptr), cvGetElemType(self_ptr));
    cvPyrSegmentation(self_ptr, IPLIMAGE(dest), CVMEMSTORAGE(storage), &comp,
		      NUM2INT(level), NUM2DBL(threshold1), NUM2DBL(threshold2));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  if (!comp)
    comp = cvCreateSeq(CV_SEQ_CONNECTED_COMP, sizeof(CvSeq), sizeof(CvConnectedComp), CVMEMSTORAGE(storage));
  return rb_ary_new3(2, dest, cCvSeq::new_sequence(cCvSeq::rb_class(), comp, cCvConnectedComp::rb_class(), storage));
}

/*
 * call-seq:
 *   pyr_mean_shift_filtering(<i>sp, sr[,max_level = 1][termcrit = CvTermCriteria.new(5,1)]</i>) -> cvmat
 *
 * Does meanshift image segmentation.
 *
 *   sp - The spatial window radius.
 *   sr - The color window radius.
 *   max_level - Maximum level of the pyramid for the segmentation.
 *   termcrit - Termination criteria: when to stop meanshift iterations.
 *
 * This method is implements the filtering stage of meanshift segmentation,
 * that is, the output of the function is the filtered "posterized" image with color gradients and fine-grain texture flattened.
 * At every pixel (X,Y) of the input image (or down-sized input image, see below)
 * the function executes meanshift iterations, that is, the pixel (X,Y) neighborhood in the joint space-color hyperspace is considered:
 *   {(x,y): X-sp≤x≤X+sp && Y-sp≤y≤Y+sp && ||(R,G,B)-(r,g,b)|| ≤ sr},
 * where (R,G,B) and (r,g,b) are the vectors of color components at (X,Y) and (x,y),
 * respectively (though, the algorithm does not depend on the color space used,
 * so any 3-component color space can be used instead).
 * Over the neighborhood the average spatial value (X',Y')
 * and average color vector (R',G',B') are found and they act as the neighborhood center on the next iteration:
 *   (X,Y)~(X',Y'), (R,G,B)~(R',G',B').
 * After the iterations over, the color components of the initial pixel (that is, the pixel from where the iterations started)
 * are set to the final value (average color at the last iteration):
 *   I(X,Y) <- (R*,G*,B*).
 * Then max_level > 0, the gaussian pyramid of max_level+1 levels is built,
 * and the above procedure is run on the smallest layer.
 * After that, the results are propagated to the larger layer and the iterations are run again
 * only on those pixels where the layer colors differ much (>sr) from the lower-resolution layer,
 * that is, the boundaries of the color regions are clarified.
 *
 * Note, that the results will be actually different from the ones obtained by running the meanshift procedure on the whole original image (i.e. when max_level==0).
 */
VALUE
rb_pyr_mean_shift_filtering(int argc, VALUE *argv, VALUE self)
{
  VALUE spatial_window_radius, color_window_radius, max_level, termcrit;
  rb_scan_args(argc, argv, "22", &spatial_window_radius, &color_window_radius, &max_level, &termcrit);
  CvArr* self_ptr = CVARR(self);
  VALUE dest = Qnil;
  try {
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvPyrMeanShiftFiltering(self_ptr, CVARR(dest),
			    NUM2DBL(spatial_window_radius),
			    NUM2DBL(color_window_radius),
			    IF_INT(max_level, 1),
			    NIL_P(termcrit) ? cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 5, 1)
			    : VALUE_TO_CVTERMCRITERIA(termcrit));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   watershed -> cvmat(mean markers:cv32s)
 *
 * Does watershed segmentation.
 */
VALUE
rb_watershed(VALUE self, VALUE markers)
{
  try {
    cvWatershed(CVARR(self), CVARR_WITH_CHECK(markers));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return markers;
}

/*
 * call-seq:
 *   moments -> array(include CvMoments)
 *
 * Calculates moments.
 */
VALUE
rb_moments(int argc, VALUE *argv, VALUE self)
{
  VALUE is_binary;
  rb_scan_args(argc, argv, "01", &is_binary);
  IplImage image = *IPLIMAGE(self);
  VALUE moments = rb_ary_new();
  try {
    int cn = CV_MAT_CN(cvGetElemType(CVARR(self)));
    for (int i = 1; i <= cn; ++i) {
      cvSetImageCOI(&image, i);
      rb_ary_push(moments, cCvMoments::new_object(&image, TRUE_OR_FALSE(is_binary, 0)));
    }
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return moments;
}

/*
 * call-seq:
 *   hough_lines(<i>method, rho, theta, threshold, param1, param2</i>) -> cvseq(include CvLine or CvTwoPoints)
 *
 * Finds lines in binary image using a Hough transform.
 * * method –
 * *   The Hough transform variant, one of the following:
 * *   - CV_HOUGH_STANDARD - classical or standard Hough transform.
 * *   - CV_HOUGH_PROBABILISTIC - probabilistic Hough transform (more efficient in case if picture contains a few long linear segments).
 * *   - CV_HOUGH_MULTI_SCALE - multi-scale variant of the classical Hough transform. The lines are encoded the same way as CV_HOUGH_STANDARD.
 * * rho - Distance resolution in pixel-related units.
 * * theta - Angle resolution measured in radians.
 * * threshold - Threshold parameter. A line is returned by the function if the corresponding accumulator value is greater than threshold.
 * * param1 –
 * *   The first method-dependent parameter:
 * *     For the classical Hough transform it is not used (0).
 * *     For the probabilistic Hough transform it is the minimum line length.
 * *     For the multi-scale Hough transform it is the divisor for the distance resolution . (The coarse distance resolution will be rho and the accurate resolution will be (rho / param1)).
 * * param2 –
 * *   The second method-dependent parameter:
 * *     For the classical Hough transform it is not used (0).
 * *     For the probabilistic Hough transform it is the maximum gap between line segments lying on the same line to treat them as a single line segment (i.e. to join them).
 * *     For the multi-scale Hough transform it is the divisor for the angle resolution. (The coarse angle resolution will be theta and the accurate resolution will be (theta / param2).)
 */
VALUE
rb_hough_lines(int argc, VALUE *argv, VALUE self)
{
  const int INVALID_TYPE = -1;
  VALUE method, rho, theta, threshold, p1, p2;
  rb_scan_args(argc, argv, "42", &method, &rho, &theta, &threshold, &p1, &p2);
  int method_flag = CVMETHOD("HOUGH_TRANSFORM_METHOD", method, INVALID_TYPE);
  if (method_flag == INVALID_TYPE)
    rb_raise(rb_eArgError, "Invalid method: %d", method_flag);
  VALUE storage = cCvMemStorage::new_object();
  CvSeq *seq = NULL;
  try {
    seq = cvHoughLines2(CVARR(copy(self)), CVMEMSTORAGE(storage),
			method_flag, NUM2DBL(rho), NUM2DBL(theta), NUM2INT(threshold),
			IF_DBL(p1, 0), IF_DBL(p2, 0));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  switch (method_flag) {
  case CV_HOUGH_STANDARD:
  case CV_HOUGH_MULTI_SCALE:
    return cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvLine::rb_class(), storage);
    break;
  case CV_HOUGH_PROBABILISTIC:
    return cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvTwoPoints::rb_class(), storage);
    break;
  default:
    break;
  }

  return Qnil;
}

/*
 * call-seq:
 *   hough_circles(<i>method, dp, min_dist, param1, param2, min_radius = 0, max_radius = max(width,height)</i>) -> cvseq(include CvCircle32f)
 *
 * Finds circles in grayscale image using Hough transform.
 */
VALUE
rb_hough_circles(int argc, VALUE *argv, VALUE self)
{
  const int INVALID_TYPE = -1;
  VALUE method, dp, min_dist, param1, param2, min_radius, max_radius, storage;
  rb_scan_args(argc, argv, "52", &method, &dp, &min_dist, &param1, &param2, 
	       &min_radius, &max_radius);
  storage = cCvMemStorage::new_object();
  int method_flag = CVMETHOD("HOUGH_TRANSFORM_METHOD", method, INVALID_TYPE);
  if (method_flag == INVALID_TYPE)
    rb_raise(rb_eArgError, "Invalid method: %d", method_flag);
  CvSeq *seq = NULL;
  try {
    seq = cvHoughCircles(CVARR(self), CVMEMSTORAGE(storage),
			 method_flag, NUM2DBL(dp), NUM2DBL(min_dist),
			 NUM2DBL(param1), NUM2DBL(param2),
			 IF_INT(min_radius, 0), IF_INT(max_radius, 0));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvCircle32f::rb_class(), storage);
}

/*
 * call-seq:
 *   inpaint(<i>inpaint_method, mask, radius</i>) -> cvmat
 *
 * Inpaints the selected region in the image
 * The radius of circlular neighborhood of each point inpainted that is considered by the algorithm.
 */
VALUE
rb_inpaint(VALUE self, VALUE inpaint_method, VALUE mask, VALUE radius)
{
  const int INVALID_TYPE = -1;
  VALUE dest = Qnil;
  int method = CVMETHOD("INPAINT_METHOD", inpaint_method, INVALID_TYPE);
  if (method == INVALID_TYPE)
    rb_raise(rb_eArgError, "Invalid method");
  try {
    CvArr* self_ptr = CVARR(self);
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvInpaint(self_ptr, MASK(mask), CVARR(dest), NUM2DBL(radius), method);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   equalize_hist - cvmat
 *
 * Equalize histgram of grayscale of image.
 *
 * equalizes histogram of the input image using the following algorithm:
 * 1. calculate histogram H for src.
 * 2. normalize histogram, so that the sum of histogram bins is 255.
 * 3. compute integral of the histogram:
 *    H’(i) = sum0≤j≤iH(j)
 * 4. transform the image using H’ as a look-up table: dst(x,y)=H’(src(x,y))
 * The algorithm normalizes brightness and increases contrast of the image.
 *
 * <b>support single-channel 8bit image (grayscale) only.</b>
 */
VALUE
rb_equalize_hist(VALUE self)
{
  VALUE dest = Qnil;
  try {
    CvArr* self_ptr = CVARR(self);
    dest = new_mat_kind_object(cvGetSize(self_ptr), self);
    cvEqualizeHist(self_ptr, CVARR(dest));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return dest;
}

/*
 * call-seq:
 *   match_template(<i>template[,method = :sqdiff]</i>) -> cvmat(result)
 *
 * Compares template against overlapped image regions.

 * <i>method</i> is specifies the way the template must be compared with image regions.
 * <i>method</i> should be following symbol. (see OpenCV::MATCH_TEMPLATE_METHOD 's key and value.)
 *
 * * :sqdiff
 *   R(x,y)=sumx',y'[T(x',y')-I(x+x',y+y')]2
 * * :sqdiff_normed
 *   R(x,y)=sumx',y'[T(x',y')-I(x+x',y+y')]2/sqrt[sumx',y'T(x',y')2*sumx',y'I(x+x',y+y')2]
 * * :ccorr
 *   R(x,y)=sumx',y'[T(x',y')*I(x+x',y+y')]
 * * :ccorr_normed
 *   R(x,y)=sumx',y'[T(x',y')*I(x+x',y+y')]/sqrt[sumx',y'T(x',y')2*sumx',y'I(x+x',y+y')2]
 * * :ccoeff
 *   R(x,y)=sumx',y'[T'(x',y')*I'(x+x',y+y')],
 *     where T'(x',y')=T(x',y') - 1/(w*h)*sumx",y"T(x",y")
 *   I'(x+x',y+y')=I(x+x',y+y') - 1/(w*h)*sumx",y"I(x+x",y+y")
 * * :ccoeff_normed
 *   R(x,y)=sumx',y'[T'(x',y')*I'(x+x',y+y')]/sqrt[sumx',y'T'(x',y')2*sumx',y'I'(x+x',y+y')2]
 *
 * After the match_template finishes comparison, the best matches can be found as global
 * minimums (:sqdiff*) or maximums(:ccorr* or :ccoeff*) using minmax function.
 * In case of color image and template summation in both numerator and each sum in denominator
 * is done over all the channels (and separate mean values are used for each channel).
 */
VALUE
rb_match_template(int argc, VALUE *argv, VALUE self)
{
  VALUE templ, method;
  int method_flag;
  if (rb_scan_args(argc, argv, "11", &templ, &method) == 1)
    method_flag = CV_TM_SQDIFF;
  else
    method_flag = CVMETHOD("MATCH_TEMPLATE_METHOD", method);

  CvArr* self_ptr = CVARR(self);
  CvArr* templ_ptr = CVARR_WITH_CHECK(templ);
  VALUE result = Qnil;
  try {
    CvSize src_size = cvGetSize(self_ptr);
    CvSize template_size = cvGetSize(templ_ptr);
    result = cCvMat::new_object(src_size.height - template_size.height + 1,
				src_size.width - template_size.width + 1,
				CV_32FC1);
    cvMatchTemplate(self_ptr, templ_ptr, CVARR(result), method_flag);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return result;
}

/*
 * call-seq:
 *   match_shapes(<i>object, method</i>) -> float
 *
 * Compares two shapes(self and object). <i>object</i> should be CvMat or CvContour.
 *
 * A - object1, B - object2:
 * * method=CV_CONTOUR_MATCH_I1
 *     I1(A,B)=sumi=1..7abs(1/mAi - 1/mBi)
 * * method=CV_CONTOUR_MATCH_I2
 *     I2(A,B)=sumi=1..7abs(mAi - mBi)
 * * method=CV_CONTOUR_MATCH_I3
 *     I3(A,B)=sumi=1..7abs(mAi - mBi)/abs(mAi)
 */
VALUE
rb_match_shapes(int argc, VALUE *argv, VALUE self)
{
  VALUE object, method, param;
  rb_scan_args(argc, argv, "21", &object, &method, &param);
  int method_flag = CVMETHOD("COMPARISON_METHOD", method);
  if (!(rb_obj_is_kind_of(object, cCvMat::rb_class()) || rb_obj_is_kind_of(object, cCvContour::rb_class())))
    rb_raise(rb_eTypeError, "argument 1 (shape) should be %s or %s",
	     rb_class2name(cCvMat::rb_class()), rb_class2name(cCvContour::rb_class()));
  double result = 0;
  try {
    result = cvMatchShapes(CVARR(self), CVARR(object), method_flag);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_float_new(result);
}

/*
 * call-seq:
 *   mean_shift(window, criteria) -> comp
 *
 * Implements CAMSHIFT object tracking algrorithm.
 * First, it finds an object center using mean_shift and, after that,
 * calculates the object size and orientation.
 */
VALUE
rb_mean_shift(VALUE self, VALUE window, VALUE criteria)
{
  VALUE comp = cCvConnectedComp::new_object();
  try {
    cvMeanShift(CVARR(self), VALUE_TO_CVRECT(window), VALUE_TO_CVTERMCRITERIA(criteria), CVCONNECTEDCOMP(comp));    
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return comp;
}

/*
 * call-seq:
 *   cam_shift(window, criteria) -> [comp, box]
 *
 * Implements CAMSHIFT object tracking algrorithm. First, it finds an object center using cvMeanShift and,
 * after that, calculates the object size and orientation. The function returns number of iterations made
 * within cvMeanShift.
 */
VALUE
rb_cam_shift(VALUE self, VALUE window, VALUE criteria)
{
  VALUE comp = cCvConnectedComp::new_object();
  VALUE box = cCvBox2D::new_object();
  try {
    cvCamShift(CVARR(self), VALUE_TO_CVRECT(window), VALUE_TO_CVTERMCRITERIA(criteria),
	       CVCONNECTEDCOMP(comp), CVBOX2D(box));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(2, comp, box);
}

/*
 * call-seq:
 *   snake_image(<i>points, alpha, beta, gamma, window, criteria[, calc_gradient = true]</i>) -> array(pointset)
 *
 * Updates snake in order to minimize its total energy that is a sum of internal energy
 * that depends on contour shape (the smoother contour is, the smaller internal energy is)
 * and external energy that depends on the energy field and reaches minimum at the local energy
 * extremums that correspond to the image edges in case of image gradient.

 * The parameter criteria.epsilon is used to define the minimal number of points that must be moved
 * during any iteration to keep the iteration process running.
 *
 * If at some iteration the number of moved points is less than criteria.epsilon or
 * the function performed criteria.max_iter iterations, the function terminates.
 *
 * points
 *   Contour points (snake).
 * alpha
 *   Weight[s] of continuity energy, single float or array of length floats, one per each contour point.
 * beta
 *   Weight[s] of curvature energy, similar to alpha.
 * gamma
 *   Weight[s] of image energy, similar to alpha.
 * window
 *   Size of neighborhood of every point used to search the minimum, both win.width and win.height must be odd.
 * criteria
 *  Termination criteria.
 * calc_gradient
 *  Gradient flag. If not 0, the function calculates gradient magnitude for every image pixel and consideres
 *  it as the energy field, otherwise the input image itself is considered.
 */
VALUE
rb_snake_image(int argc, VALUE *argv, VALUE self)
{
  VALUE points, alpha, beta, gamma, window, criteria, calc_gradient;
  rb_scan_args(argc, argv, "61", &points, &alpha, &beta, &gamma, &window, &criteria, &calc_gradient);
  CvPoint *pointset = 0;
  int length = CVPOINTS_FROM_POINT_SET(points, &pointset);
  int coeff = (TYPE(alpha) == T_ARRAY && TYPE(beta) == T_ARRAY && TYPE(gamma) == T_ARRAY) ? CV_ARRAY : CV_VALUE;
  float *a = 0, *b = 0, *c = 0;
  IplImage stub;
  int i;
  if (coeff == CV_VALUE) {
    float buff_a, buff_b, buff_c;
    buff_a = (float)NUM2DBL(alpha);
    buff_b = (float)NUM2DBL(beta);
    buff_c = (float)NUM2DBL(gamma);
    a = &buff_a;
    b = &buff_b;
    c = &buff_c;
  }
  else { // CV_ARRAY
    if ((RARRAY_LEN(alpha) != length) ||
	(RARRAY_LEN(beta) != length) ||
	(RARRAY_LEN(gamma) != length))
      rb_raise(rb_eArgError, "alpha, beta, gamma should be same size of points");
    a = ALLOCA_N(float, length);
    b = ALLOCA_N(float, length);
    c = ALLOCA_N(float, length);
    for (i = 0; i < length; ++i) {
      a[i] = (float)NUM2DBL(RARRAY_PTR(alpha)[i]);
      b[i] = (float)NUM2DBL(RARRAY_PTR(beta)[i]);
      c[i] = (float)NUM2DBL(RARRAY_PTR(gamma)[i]);
    }
  }
  CvSize win = VALUE_TO_CVSIZE(window);
  CvTermCriteria tc = VALUE_TO_CVTERMCRITERIA(criteria);
  try {
    cvSnakeImage(cvGetImage(CVARR(self), &stub), pointset, length,
		 a, b, c, coeff, win, tc, IF_BOOL(calc_gradient, 1, 0, 1));
  }
  catch (cv::Exception& e) {
    if (pointset != NULL)
      cvFree(&pointset);
    raise_cverror(e);
  }
  VALUE result = rb_ary_new2(length);
  for (i = 0; i < length; ++i)
    rb_ary_push(result, cCvPoint::new_object(pointset[i]));
  cvFree(&pointset);
  
  return result;
}

/*
 * call-seq:
 *   optical_flow_hs(<i>prev[,velx = nil][,vely = nil][,options]</i>) -> [cvmat, cvmat]
 *
 * Calculates optical flow for two images (previous -> self) using Horn & Schunck algorithm.
 * Return horizontal component of the optical flow and vertical component of the optical flow.
 * <i>prev</i> is previous image
 * <i>velx</i> is previous velocity field of x-axis, and <i>vely</i> is previous velocity field of y-axis.
 *
 * options
 * * :lambda -> should be Float (default is 0.0005)
 *     Lagrangian multiplier.
 * * :criteria -> should be CvTermCriteria object (default is CvTermCriteria(1, 0.001))
 *     Criteria of termination of velocity computing.
 * note: <i>option</i>'s default value is CvMat::OPTICAL_FLOW_HS_OPTION.
 *
 * sample code
 *   velx, vely = nil, nil
 *   while true
 *     current = capture.query
 *     velx, vely = current.optical_flow_hs(prev, velx, vely) if prev
 *     prev = current
 *   end
 */
VALUE
rb_optical_flow_hs(int argc, VALUE *argv, VALUE self)
{
  VALUE prev, velx, vely, options;
  int use_previous = 0;
  rb_scan_args(argc, argv, "13", &prev, &velx, &vely, &options);
  options = OPTICAL_FLOW_HS_OPTION(options);
  CvMat *velx_ptr, *vely_ptr;
  CvArr* self_ptr = CVARR(self);
  try {
    if (NIL_P(velx) && NIL_P(vely)) {
      CvSize size = cvGetSize(self_ptr);
      int type = CV_MAKETYPE(CV_32F, 1);
      velx = cCvMat::new_object(size, type);
      vely = cCvMat::new_object(size, type);
      velx_ptr = CVMAT(velx);
      vely_ptr = CVMAT(vely);
    }
    else {
      use_previous = 1;
      velx_ptr = CVMAT_WITH_CHECK(velx);
      vely_ptr = CVMAT_WITH_CHECK(vely);
    }
    cvCalcOpticalFlowHS(CVMAT_WITH_CHECK(prev), self_ptr, use_previous, velx_ptr, vely_ptr,
			HS_LAMBDA(options), HS_CRITERIA(options));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(2, velx, vely);
}

/*
 * call-seq:
 *   optical_flow_lk(<i>prev, win_size</i>) -> [cvmat, cvmat]
 *
 * Calculates optical flow for two images (previous -> self) using Lucas & Kanade algorithm
 * Return horizontal component of the optical flow and vertical component of the optical flow.
 *
 * <i>win_size</i> is size of the averaging window used for grouping pixels.
 */
VALUE
rb_optical_flow_lk(VALUE self, VALUE prev, VALUE win_size)
{
  VALUE velx = Qnil;
  VALUE vely = Qnil;
  try {
    CvArr* self_ptr = CVARR(self);
    CvSize size = cvGetSize(self_ptr);
    int type = CV_MAKETYPE(CV_32F, 1);
    velx = cCvMat::new_object(size, type);
    vely = cCvMat::new_object(size, type);
    cvCalcOpticalFlowLK(CVMAT_WITH_CHECK(prev), self_ptr, VALUE_TO_CVSIZE(win_size),
			CVARR(velx), CVARR(vely));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(2, velx, vely);
}

/*
 * call-seq:
 *   optical_flow_bm(<i>prev[,velx = nil][,vely = nil][,option]</i>) -> [cvmat, cvmat]
 *
 * Calculates optical flow for two images (previous -> self) using block matching method.
 * Return horizontal component of the optical flow and vertical component of the optical flow.
 * <i>prev</i> is previous image.
 * <i>velx</i> is previous velocity field of x-axis, and <i>vely</i> is previous velocity field of y-axis.
 *
 * options
 * * :block_size -> should be CvSize (default is CvSize(4,4))
 *     Size of basic blocks that are compared.
 * * :shift_size -> should be CvSize (default is CvSize(1,1))
 *     Block coordinate increments.
 * * :max_range -> should be CvSize (default is CVSize(4,4))
 *     Size of the scanned neighborhood in pixels around block.
 * note: <i>option</i>'s default value is CvMat::OPTICAL_FLOW_BM_OPTION.
 *
 * Velocity is computed for every block, but not for every pixel,
 * so velocity image pixels correspond to input image blocks.
 * input/output velocity field's size should be (self.width / block_size.width)x(self.height / block_size.height).
 * e.g. image.size is 320x240 and block_size is 4x4, velocity field's size is 80x60.
 *
 */
VALUE
rb_optical_flow_bm(int argc, VALUE *argv, VALUE self)
{
  VALUE prev, velx, vely, options;
  rb_scan_args(argc, argv, "13", &prev, &velx, &vely, &options);
  options = OPTICAL_FLOW_BM_OPTION(options);
  CvArr* self_ptr = CVARR(self);
  CvSize block_size = BM_BLOCK_SIZE(options);
  CvSize shift_size = BM_SHIFT_SIZE(options);
  CvSize max_range  = BM_MAX_RANGE(options);

  int use_previous = 0;
  try {
    CvSize image_size = cvGetSize(self_ptr);
    CvSize velocity_size = cvSize((image_size.width - block_size.width + shift_size.width) / shift_size.width,
				  (image_size.height - block_size.height + shift_size.height) / shift_size.height);
    CvMat *velx_ptr, *vely_ptr;
    if (NIL_P(velx) && NIL_P(vely)) {
      int type = CV_MAKETYPE(CV_32F, 1);
      velx = cCvMat::new_object(velocity_size, type);
      vely = cCvMat::new_object(velocity_size, type);
      velx_ptr = CVMAT(velx);
      vely_ptr = CVMAT(vely);
    }
    else {
      use_previous = 1;
      velx_ptr = CVMAT_WITH_CHECK(velx);
      vely_ptr = CVMAT_WITH_CHECK(vely);
    }
    cvCalcOpticalFlowBM(CVMAT_WITH_CHECK(prev), self_ptr,
			block_size, shift_size, max_range, use_previous,
			velx_ptr, vely_ptr);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return rb_ary_new3(2, velx, vely);
}

/*
 * call-seq:
 *   CvMat.find_fundamental_mat(<i>points1, points2[,options = {}]</i>) -> fundamental_matrix(cvmat) or nil
 *
 * Calculates fundamental matrix from corresponding points.
 * Size of the output fundamental matrix is 3x3 or 9x3 (7-point method may return up to 3 matrices)
 *
 * <i>points1</i> and <i>points2</i> should be 2xN, Nx2, 3xN or Nx3 1-channel, or 1xN or Nx1 multi-channel matrix.
 * <i>method<i> is method for computing the fundamental matrix
 *    - CV_FM_7POINT for a 7-point algorithm. (N = 7)
 *    - CV_FM_8POINT for an 8-point algorithm. (N >= 8)
 *    - CV_FM_RANSAC for the RANSAC algorithm. (N >= 8)
 *    - CV_FM_LMEDS for the LMedS algorithm. (N >= 8)
 * <i>option</i> should be Hash include these keys.
 *   :with_status (true or false)
 *      If set true, return fundamental_matrix and status. [fundamental_matrix, status]
 *      Otherwise return fundamental matrix only(default).
 *   :maximum_distance
 *      The parameter is used for RANSAC.  It is the maximum distance from point to epipolar line in pixels, beyond which the point is considered an outlier and is not used for computing the final fundamental matrix. It can be set to something like 1-3, depending on the accuracy of the point localization, image resolution and the image noise.
 *   :desirable_level
 *      The optional output array of N elements, every element of which is set to 0 for outliers and to 1 for the other points. The array is computed only in RANSAC and LMedS methods. For other methods it is set to all 1's.
 *
 * note: <i>option</i>'s default value is CvMat::FIND_FUNDAMENTAL_MAT_OPTION.
 */
VALUE
rb_find_fundamental_mat(int argc, VALUE *argv, VALUE klass)
{
  VALUE points1, points2, method, option, fundamental_matrix, status;
  int num = 0;
  rb_scan_args(argc, argv, "31", &points1, &points2, &method, &option);
  option = FIND_FUNDAMENTAL_MAT_OPTION(option);
  int fm_method = FIX2INT(method);
  CvMat *points1_ptr = CVMAT_WITH_CHECK(points1);
  if (fm_method == CV_FM_7POINT)
    fundamental_matrix = cCvMat::new_object(9, 3, CV_MAT_DEPTH(points1_ptr->type));
  else
    fundamental_matrix = cCvMat::new_object(3, 3, CV_MAT_DEPTH(points1_ptr->type));

  if (FFM_WITH_STATUS(option)) {
    int status_len = (points1_ptr->rows > points1_ptr->cols) ? points1_ptr->rows : points1_ptr->cols;
    status = cCvMat::new_object(1, status_len, CV_8UC1);
    try {
      num = cvFindFundamentalMat(points1_ptr, CVMAT_WITH_CHECK(points2), CVMAT(fundamental_matrix), fm_method,
				 FFM_MAXIMUM_DISTANCE(option), FFM_DESIRABLE_LEVEL(option), CVMAT(status));
    }
    catch (cv::Exception& e) {
      raise_cverror(e);
    }
    return num == 0 ? Qnil : rb_ary_new3(2, fundamental_matrix, status);
  }
  else {
    try {
      num = cvFindFundamentalMat(points1_ptr, CVMAT_WITH_CHECK(points2), CVMAT(fundamental_matrix), fm_method,
				 FFM_MAXIMUM_DISTANCE(option), FFM_DESIRABLE_LEVEL(option), NULL);
    }
    catch (cv::Exception& e) {
      raise_cverror(e);
    }
    return num == 0 ? Qnil : fundamental_matrix;
  }
}


/*
 * call-seq:
 *   CvMat.compute_correspond_epilines(<i>points, which_image, fundamental_matrix</i>) -> correspondent_lines(cvmat)
 *
 * For points in one image of stereo pair computes the corresponding epilines in the other image.
 * Finds equation of a line that contains the corresponding point (i.e. projection of the same 3D point)
 * in the other image. Each line is encoded by a vector of 3 elements l=[a,b,c]T, so that:
 *   lT*[x, y, 1]T=0,
 * or
 *   a*x + b*y + c = 0
 * From the fundamental matrix definition (see cvFindFundamentalMatrix discussion), line l2 for a point p1 in the first image (which_image=1) can be computed as:
 *   l2=F*p1
 * and the line l1 for a point p2 in the second image (which_image=1) can be computed as:
 *   l1=FT*p2
 * Line coefficients are defined up to a scale. They are normalized (a2+b2=1) are stored into correspondent_lines.
 */
VALUE
rb_compute_correspond_epilines(VALUE klass, VALUE points, VALUE which_image, VALUE fundamental_matrix)
{
  VALUE correspondent_lines;
  CvMat* points_ptr = CVMAT_WITH_CHECK(points);
  int n;
  if (points_ptr->cols <= 3 && points_ptr->rows >= 7)
    n = points_ptr->rows;
  else if (points_ptr->rows <= 3 && points_ptr->cols >= 7)
    n = points_ptr->cols;
  else
    rb_raise(rb_eArgError, "input points should 2xN, Nx2 or 3xN, Nx3 matrix(N >= 7).");
  
  correspondent_lines = cCvMat::new_object(n, 3, CV_MAT_DEPTH(points_ptr->type));
  try {
    cvComputeCorrespondEpilines(points_ptr, NUM2INT(which_image), CVMAT_WITH_CHECK(fundamental_matrix),
				CVMAT(correspondent_lines));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return correspondent_lines;
}

/*
 * call-seq:
 *   extract_surf(params[,mask]) -> [cvseq(cvsurfpoint), array(float)]
 * Extracts Speeded Up Robust Features from an image
 *
 * <i>params</i> (CvSURFParams) - Various algorithm parameters put to the structure CvSURFParams.
 * <i>mask</i> (CvMat) - The optional input 8-bit mask. The features are only found in the areas that contain more than 50% of non-zero mask pixels.
 */
VALUE
rb_extract_surf(int argc, VALUE *argv, VALUE self)
{
  VALUE _params, _mask;
  rb_scan_args(argc, argv, "11", &_params, &_mask);

  // Prepare arguments
  CvSURFParams params = *CVSURFPARAMS_WITH_CHECK(_params);
  CvMat* mask = MASK(_mask);
  VALUE storage = cCvMemStorage::new_object();
  CvSeq* keypoints = NULL;
  CvSeq* descriptors = NULL;

  // Compute SURF keypoints and descriptors
  try {
    cvExtractSURF(CVARR(self), mask, &keypoints, &descriptors, CVMEMSTORAGE(storage),
		  params, 0);
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  VALUE _keypoints = cCvSeq::new_sequence(cCvSeq::rb_class(), keypoints, cCvSURFPoint::rb_class(), storage);
  
  // Create descriptor array
  const int DIM_SIZE = (params.extended) ? 128 : 64;
  const int NUM_KEYPOINTS = keypoints->total;
  VALUE _descriptors = rb_ary_new2(NUM_KEYPOINTS);
  for (int m = 0; m < NUM_KEYPOINTS; ++m) {
    VALUE elem = rb_ary_new2(DIM_SIZE);
    float *descriptor = (float*)cvGetSeqElem(descriptors, m);
    for (int n = 0; n < DIM_SIZE; ++n) {
      rb_ary_store(elem, n, rb_float_new(descriptor[n]));
    }
    rb_ary_store(_descriptors, m, elem);
  }
  
  return rb_assoc_new(_keypoints, _descriptors);
}

VALUE
new_object(int rows, int cols, int type)
{
  return OPENCV_OBJECT(rb_klass, rb_cvCreateMat(rows, cols, type));
}

VALUE
new_object(CvSize size, int type)
{
  return OPENCV_OBJECT(rb_klass, rb_cvCreateMat(size.height, size.width, type));
}

VALUE
new_mat_kind_object(CvSize size, VALUE ref_obj)
{
  VALUE return_type = CLASS_OF(ref_obj);
  if (rb_obj_is_kind_of(ref_obj, cIplImage::rb_class())) {
    IplImage* img = IPLIMAGE(ref_obj);
    return OPENCV_OBJECT(return_type, rb_cvCreateImage(size, img->depth, img->nChannels));
  }
  else if (rb_obj_is_kind_of(ref_obj, rb_klass)) // CvMat
    return OPENCV_OBJECT(return_type, rb_cvCreateMat(size.height, size.width, cvGetElemType(CVMAT(ref_obj))));
  else
    rb_raise(rb_eNotImpError, "Only CvMat or IplImage are supported");

  return Qnil;
}

VALUE
new_mat_kind_object(CvSize size, VALUE ref_obj, int cvmat_depth, int channel)
{
  VALUE return_type = CLASS_OF(ref_obj);
  if (rb_obj_is_kind_of(ref_obj, cIplImage::rb_class())) {
    return OPENCV_OBJECT(return_type, rb_cvCreateImage(size, CV2IPL_DEPTH(cvmat_depth), channel));
  }
  else if (rb_obj_is_kind_of(ref_obj, rb_klass)) // CvMat
    return OPENCV_OBJECT(return_type, rb_cvCreateMat(size.height, size.width,
						     CV_MAKETYPE(cvmat_depth, channel)));
  else
    rb_raise(rb_eNotImpError, "Only CvMat or IplImage are supported");

  return Qnil;
}

__NAMESPACE_END_OPENCV
__NAMESPACE_END_CVMAT
