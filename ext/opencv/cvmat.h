/************************************************************

   cvmat.h -

   $Author: lsxi $

   Copyright (C) 2005-2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVMAT_H
#define RUBY_OPENCV_CVMAT_H

#include "opencv.h"


#define __NAMESPACE_BEGIN_CVMAT namespace cCvMat {
#define __NAMESPACE_END_CVMAT }

__NAMESPACE_BEGIN_OPENCV
__NAMESPACE_BEGIN_CVMAT

VALUE rb_class();

void define_ruby_class();

VALUE rb_allocate(VALUE klass);
VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
VALUE rb_load_imageM(int argc, VALUE *argv, VALUE self);
VALUE rb_encode_imageM(int argc, VALUE *argv, VALUE self);
VALUE rb_decode_imageM(int argc, VALUE *argv, VALUE self);

VALUE rb_method_missing(int argc, VALUE *argv, VALUE self);
VALUE rb_to_s(VALUE self);
VALUE rb_inside_q(VALUE self, VALUE object);
VALUE rb_to_IplConvKernel(VALUE self, VALUE anchor);
VALUE rb_create_mask(VALUE self);

VALUE rb_width(VALUE self);
VALUE rb_height(VALUE self);
VALUE rb_depth(VALUE self);
VALUE rb_channel(VALUE self);
VALUE rb_data(VALUE self);

VALUE rb_clone(VALUE self);
VALUE rb_copy(int argc, VALUE *argv, VALUE self);
VALUE copy(VALUE mat);

VALUE rb_to_8u(VALUE self);
VALUE rb_to_8s(VALUE self);
VALUE rb_to_16u(VALUE self);
VALUE rb_to_16s(VALUE self);
VALUE rb_to_32s(VALUE self);
VALUE rb_to_32f(VALUE self);
VALUE rb_to_64f(VALUE self);
VALUE rb_vector_q(VALUE self);
VALUE rb_square_q(VALUE self);
// cxcore function
VALUE rb_to_CvMat(VALUE self);
VALUE rb_to_IplImage(VALUE self);
VALUE rb_sub_rect(VALUE self, VALUE args);
VALUE rb_get_rows(VALUE self, VALUE args);
VALUE rb_get_cols(VALUE self, VALUE args);
VALUE rb_each_row(VALUE self);
VALUE rb_each_col(VALUE self);
VALUE rb_diag(int argc, VALUE *argv, VALUE self);
VALUE rb_size(VALUE self);
VALUE rb_dims(VALUE self);
VALUE rb_dim_size(VALUE self, VALUE index);
VALUE rb_aref(VALUE self, VALUE args);
VALUE rb_aset(VALUE self, VALUE args);
VALUE rb_set_data(VALUE self, VALUE data);
VALUE rb_fill(int argc, VALUE *argv, VALUE self);
VALUE rb_fill_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_clear(VALUE self);
VALUE rb_clear_bang(VALUE self);
VALUE rb_set_identity(int argc, VALUE *argv, VALUE self);
VALUE rb_set_identity_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_range(VALUE self, VALUE start, VALUE end);
VALUE rb_range_bang(VALUE self, VALUE start, VALUE end);
/* Transforms and Permutations */
VALUE rb_reshape(VALUE self, VALUE hash);
VALUE rb_repeat(VALUE self, VALUE object);
VALUE rb_flip(int argc, VALUE *argv, VALUE self);
VALUE rb_flip_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_split(VALUE self);
VALUE rb_merge(VALUE klass, VALUE args);
VALUE rb_rand_shuffle(int argc, VALUE *argv, VALUE klass);
VALUE rb_rand_shuffle_bang(int argc, VALUE *argv, VALUE klass);

VALUE rb_lut(VALUE self, VALUE lut);
VALUE rb_convert_scale(VALUE self, VALUE hash);
VALUE rb_convert_scale_abs(VALUE self, VALUE hash);
VALUE rb_add(int argc, VALUE *argv, VALUE self);
VALUE rb_sub(int argc, VALUE *argv, VALUE self);    
VALUE rb_mul(int argc, VALUE *argv, VALUE self);
VALUE rb_mat_mul(int argc, VALUE *argv, VALUE self);
VALUE rb_div(int argc, VALUE *argv, VALUE self);    
VALUE rb_and(int argc, VALUE *argv, VALUE self);
VALUE rb_or(int argc, VALUE *argv, VALUE self);
VALUE rb_xor(int argc, VALUE *argv, VALUE self);
VALUE rb_not(VALUE self);
VALUE rb_not_bang(VALUE self);
VALUE rb_cmp_internal(VALUE self, VALUE val, int operand);
VALUE rb_eq(VALUE self, VALUE val);
VALUE rb_gt(VALUE self, VALUE val);
VALUE rb_ge(VALUE self, VALUE val);
VALUE rb_lt(VALUE self, VALUE val);
VALUE rb_le(VALUE self, VALUE val);
VALUE rb_ne(VALUE self, VALUE val);
VALUE rb_in_range(VALUE self, VALUE min, VALUE max);
VALUE rb_abs_diff(VALUE self, VALUE val);
VALUE rb_normalize(int argc, VALUE *argv, VALUE self);
VALUE rb_add_weighted(VALUE klass, VALUE src1, VALUE alpha, VALUE src2, VALUE beta, VALUE gamma);
/* Statistics */
VALUE rb_count_non_zero(VALUE self);
VALUE rb_sum(VALUE self);
VALUE rb_avg(int argc, VALUE *argv, VALUE self);
VALUE rb_avg_sdv(int argc, VALUE *argv, VALUE self);
VALUE rb_sdv(int argc, VALUE *argv, VALUE self);
VALUE rb_min_max_loc(int argc, VALUE *argv, VALUE self);

//VALUE rb_norm();
VALUE rb_dot_product(VALUE self, VALUE mat);
VALUE rb_cross_product(VALUE self, VALUE mat);
// VALUE rb_gemm();
VALUE rb_transform(int argc, VALUE *argv, VALUE self);
VALUE rb_perspective_transform(VALUE self, VALUE mat);
VALUE rb_mul_transposed(int argc, VALUE *argv, VALUE self);
VALUE rb_trace(VALUE self);
VALUE rb_transpose(VALUE self);
VALUE rb_det(VALUE self);
VALUE rb_invert(int argc, VALUE *argv, VALUE self);
VALUE rb_solve(int argc, VALUE *argv, VALUE self);
VALUE rb_svd(int argc, VALUE *argv, VALUE self);
VALUE rb_svbksb(int argc, VALUE *argv, VALUE self);
VALUE rb_eigenvv(int argc, VALUE *argv, VALUE self);
VALUE rb_eigenvv_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_calc_covar_matrix(int argc, VALUE *argv, VALUE self);
VALUE rb_mahalonobis(int argc, VALUE *argv, VALUE self);

VALUE rb_dft(int argc, VALUE *argv, VALUE self);
VALUE rb_dct(int argc, VALUE *argv, VALUE self);

/* drawing function*/
VALUE rb_line(int argc, VALUE *argv, VALUE self);
VALUE rb_line_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_rectangle(int argc, VALUE *argv, VALUE self);
VALUE rb_rectangle_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_circle(int argc, VALUE *argv, VALUE self);
VALUE rb_circle_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_ellipse(int argc, VALUE *argv, VALUE self);
VALUE rb_ellipse_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_ellipse_box(int argc, VALUE *argv, VALUE self);
VALUE rb_ellipse_box_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_fill_poly(int argc, VALUE *argv, VALUE self);
VALUE rb_fill_poly_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_fill_convex_poly(int argc, VALUE *argv, VALUE self);
VALUE rb_fill_convex_poly_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_poly_line(int argc, VALUE *argv, VALUE self);
VALUE rb_poly_line_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_put_text(int argc, VALUE *argv, VALUE self);
VALUE rb_put_text_bang(int argc, VALUE *argv, VALUE self);

/* cv function */
VALUE rb_sobel(int argc, VALUE *argv, VALUE self);
VALUE rb_laplace(int argc, VALUE *argv, VALUE self);
VALUE rb_canny(int argc, VALUE *argv, VALUE self);
VALUE rb_pre_corner_detect(int argc, VALUE *argv, VALUE self);
VALUE rb_corner_eigenvv(int argc, VALUE *argv, VALUE self);
VALUE rb_corner_min_eigen_val(int argc, VALUE *argv, VALUE self);
VALUE rb_corner_harris(int argc, VALUE *argv, VALUE self);
VALUE rbi_find_corner_sub_pix(int argc, VALUE *argv, VALUE self);
VALUE rb_good_features_to_track(int argc, VALUE *argv, VALUE self);

VALUE rb_sample_line(int argc, VALUE *argv, VALUE self);
VALUE rb_rect_sub_pix(int argc, VALUE *argv, VALUE self);
VALUE rb_quadrangle_sub_pix(int argc, VALUE *argv, VALUE self);
VALUE rb_resize(int argc, VALUE *argv, VALUE self);
VALUE rb_warp_affine(int argc, VALUE *argv, VALUE self);
VALUE rb_rotation_matrix2D(VALUE self, VALUE center, VALUE angle, VALUE scale);
VALUE rb_warp_perspective(int argc, VALUE *argv, VALUE self);
VALUE rb_find_homograpy(int argc, VALUE *argv, VALUE self);
VALUE rb_remap(int argc, VALUE *argv, VALUE self);
VALUE rb_log_polar(int argc, VALUE *argv, VALUE self);

VALUE rb_erode(int argc, VALUE *argv, VALUE self);
VALUE rb_erode_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_dilate(int argc, VALUE *argv, VALUE self);
VALUE rb_dilate_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_morphology(int argc, VALUE *argv, VALUE self);

VALUE rb_smooth(int argc, VALUE *argv, VALUE self);
VALUE rb_copy_make_border(int argc, VALUE *argv, VALUE self);
VALUE rb_filter2d(int argc, VALUE *argv, VALUE self);
VALUE rb_integral(int argc, VALUE *argv, VALUE self);
VALUE rb_threshold(int argc, VALUE *argv, VALUE self);
VALUE rb_adaptive_threshold(int argc, VALUE *argv, VALUE self);

VALUE rb_pyr_down(int argc, VALUE *argv, VALUE self);
VALUE rb_pyr_up(int argc, VALUE *argv, VALUE self);

VALUE rb_flood_fill(int argc, VALUE *argv, VALUE self);
VALUE rb_flood_fill_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_find_contours(int argc, VALUE *argv, VALUE self);
VALUE rb_find_contours_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_draw_contours(int argc, VALUE *argv, VALUE self);
VALUE rb_draw_contours_bang(int argc, VALUE *argv, VALUE self);
VALUE rb_pyr_segmentation(VALUE self, VALUE level, VALUE threshold1, VALUE threshold2);
VALUE rb_pyr_mean_shift_filtering(int argc, VALUE *argv, VALUE self);
VALUE rb_watershed(VALUE self, VALUE markers);

VALUE rb_moments(int argc, VALUE *argv, VALUE self);

VALUE rb_hough_lines(int argc, VALUE *argv, VALUE self);
VALUE rb_hough_circles(int argc, VALUE *argv, VALUE self);
VALUE rb_dist_transform(int argc, VALUE *argv, VALUE self);
VALUE rb_inpaint(VALUE self, VALUE inpaint_method, VALUE mask, VALUE radius);

VALUE rb_equalize_hist(VALUE self);
/* Matching*/
VALUE rb_match_template(int argc, VALUE *argv, VALUE self);
VALUE rb_match_shapes(int argc, VALUE *argv, VALUE self);
/* Object Tracking */
VALUE rb_mean_shift(VALUE self, VALUE window, VALUE criteria);
VALUE rb_cam_shift(VALUE self, VALUE window, VALUE criteria);
VALUE rb_snake_image(int argc, VALUE *argv, VALUE self);
/* Optical Flow */
VALUE rb_optical_flow_hs(int argc, VALUE *argv, VALUE self);
VALUE rb_optical_flow_lk(VALUE self, VALUE prev, VALUE win_size);
VALUE rb_optical_flow_bm(int argc, VALUE *argv, VALUE self);
VALUE rb_optical_flow_pyr_lk(int argc, VALUE *argv, VALUE self);

/* Epipolar Geometory */
VALUE rb_find_fundamental_mat(int argc, VALUE *argv, VALUE klass);
VALUE rb_compute_correspond_epilines(VALUE klass, VALUE points, VALUE which_image, VALUE fundamental_matrix);

/* Feature detection and description */
VALUE rb_extract_surf(int argc, VALUE *argv, VALUE self);

// HighGUI function
VALUE rb_save_image(VALUE self, VALUE filename);

VALUE new_object(int rows, int cols, int type);
VALUE new_object(CvSize size, int type);
VALUE new_mat_kind_object(CvSize size, VALUE ref_obj);
VALUE new_mat_kind_object(CvSize size, VALUE ref_obj, int cvmat_depth, int channel);

CvMat* prepare_decoding(int argc, VALUE *argv, int* iscolor, int* need_release);

__NAMESPACE_END_CVMAT

inline CvMat*
CVMAT(VALUE object)
{
  CvMat *ptr, stub;
  Data_Get_Struct(object, CvMat, ptr);
  return cvGetMat(ptr, &stub);
}

inline CvMat*
CVMAT_WITH_CHECK(VALUE object)
{
  if (!rb_obj_is_kind_of(object, cCvMat::rb_class()))
    raise_typeerror(object, cCvMat::rb_class());
  return CVMAT(object);
}

inline CvMat*
MASK(VALUE object)
{
  if (NIL_P(object))
    return NULL;
  else {
    CvMat* obj_ptr = CVMAT_WITH_CHECK(object);
    if (CV_MAT_DEPTH(obj_ptr->type) == CV_8UC1 &&
	CV_MAT_CN(obj_ptr->type) == 1)
      return obj_ptr;
    else
      rb_raise(rb_eTypeError, "Mask should be 8bit 1-channel matrix.");
  }
}

__NAMESPACE_END_OPENCV

#endif // RUBY_OPENCV_CVMAT_H
