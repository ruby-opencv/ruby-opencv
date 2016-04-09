// -*- mode: c++; coding: utf-8 -*-
#include <sstream>
#include "opencv2/highgui.hpp"

#include "mat.hpp"
#include "mat_imgproc.hpp"
#include "mat_drawing.hpp"
#include "scalar.hpp"

#include "error.hpp"

/*
 * Document-class: Cv::Mat
 */
namespace rubyopencv {
  namespace Mat {
    void free_mat(void* ptr);
    size_t memsize_mat(const void *ptr);

    VALUE rb_klass = Qnil;
    RubyMatAllocator allocator;
    rb_data_type_t opencv_mat_type = {
      "Mat",
      { 0, free_mat, memsize_mat, 0 },
      0,
      0,
      0
    };

    cv::Mat* obj2mat(VALUE obj) {
      cv::Mat* dataptr = NULL;
      TypedData_Get_Struct(obj, cv::Mat, &opencv_mat_type, dataptr);
      return dataptr;
    }

    VALUE mat2obj(cv::Mat* ptr, VALUE klass) {
      return TypedData_Wrap_Struct(klass, &opencv_mat_type, (void*)ptr);
    }

    VALUE mat2obj(cv::Mat* ptr) {
      return mat2obj(ptr, rb_klass);
    }

    void free_mat(void* ptr) {
      if (ptr) {
	cv::Mat* dataptr = (cv::Mat*)ptr;
	dataptr->release();
      }
    }

    size_t memsize_mat(const void *ptr) {
      size_t size = 0;
      if (ptr) {
	cv::Mat* m = (cv::Mat*)ptr;
	size += sizeof(*m);
	if (m->u) {
	  size += m->u->size;
	}
      }
      return size;
    }

    VALUE rb_allocate(VALUE klass) {
      return TypedData_Wrap_Struct(klass, &opencv_mat_type, 0);
    }

    /*
     * Creates a matrix
     *
     * @overload new(rows, cols, type)
     *   @param row [Integer] Number of rows in the matrix
     *   @param col [Integer] Number of columns in the matrix
     *   @param type [Integer]
     *	   The type of the matrix elements in the form of constant <b><tt>CV_<bit depth><S|U|F></tt></b>.
     *   @return [Mat] Created matrix
     * @opencv_func cv::Mat
     * @example
     *   mat1 = Mat.new(3, 4) # Creates a 3-channels 3x4 matrix whose elements are 8bit unsigned.
     *   mat2 = Mat.new(5, 6, CV_32F) # Creates a 1-channel 5x6 matrix whose elements are 32bit float.
     */
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      VALUE row, column, type;
      rb_scan_args(argc, argv, "21", &row, &column, &type);

      cv::Mat* dataptr = NULL;
      try {
	cv::Mat tempdata(NUM2INT(row), NUM2INT(column), (NIL_P(type) ? CV_8UC1 : NUM2INT(type)));
	if (tempdata.empty()) {
	  rb_raise(rb_eNoMemError, "Failed to create matrix");
	  return Qnil;
	}

	dataptr = new cv::Mat();
	tempdata.copyTo(*dataptr);

	RTYPEDDATA_DATA(self) = dataptr;
      }
      catch (cv::Exception& e) {
	delete dataptr;
	Error::raise(e);
      }

      return self;
    }

    /**
     * Returns a zero array of the specified size and type.
     *
     * @overload zeros(rows, cols, type)
     *   @param row [Integer] Number of rows in the matrix
     *   @param col [Integer] Number of columns in the matrix
     *   @param type [Integer]
     *	   The type of the matrix elements in the form of constant <b><tt>CV_<bit depth><S|U|F></tt></b>.
     * @return [Mat] Zero array
     */
    VALUE rb_zeros(VALUE self, VALUE rows, VALUE cols, VALUE type) {
      cv::Mat* destptr = NULL;
      try {
	destptr = new cv::Mat();
	cv::Mat z = cv::Mat::zeros(NUM2INT(rows), NUM2INT(cols), NUM2INT(type));
	z.copyTo(*destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr);
    }

    /**
     * Returns an array of all 1's of the specified size and type.
     *
     * @overload ones(rows, cols, type)
     *   @param row [Integer] Number of rows in the matrix
     *   @param col [Integer] Number of columns in the matrix
     *   @param type [Integer]
     *	   The type of the matrix elements in the form of constant <b><tt>CV_<bit depth><S|U|F></tt></b>.
     * @return [Mat] Array of all 1's
     */
    VALUE rb_ones(VALUE self, VALUE rows, VALUE cols, VALUE type) {
      cv::Mat* destptr = NULL;
      try {
	destptr = new cv::Mat();
	cv::Mat z = cv::Mat::ones(NUM2INT(rows), NUM2INT(cols), NUM2INT(type));
	z.copyTo(*destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr);
    }

    /**
     * Returns an identity matrix of the specified size and type.
     *
     * @overload eye(rows, cols, type)
     *   @param row [Integer] Number of rows in the matrix
     *   @param col [Integer] Number of columns in the matrix
     *   @param type [Integer]
     *	   The type of the matrix elements in the form of constant <b><tt>CV_<bit depth><S|U|F></tt></b>.
     * @return [Mat] Identity matrix
     */
    VALUE rb_eye(VALUE self, VALUE rows, VALUE cols, VALUE type) {
      cv::Mat* destptr = NULL;
      try {
	destptr = new cv::Mat();
	cv::Mat z = cv::Mat::eye(NUM2INT(rows), NUM2INT(cols), NUM2INT(type));
	z.copyTo(*destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr);
    }

    /*
     * Makes a clone of an object.
     *
     * @overload clone
     * @return [Mat] Clone of the object
     */
    VALUE rb_clone(VALUE self) {
      VALUE clone = rb_obj_clone(self);
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* dataptr = NULL;
      try {
	dataptr = new cv::Mat();
	selfptr->copyTo(*dataptr);
	RTYPEDDATA_DATA(clone) = dataptr;
      }
      catch (cv::Exception& e) {
	delete dataptr;
	Error::raise(e);
      }

      return clone;
    }

    VALUE rb_imread_internal(VALUE self, VALUE filename, VALUE flags, VALUE klass) {
      cv::Mat* dataptr = NULL;
      try {
	cv::Mat tmp = cv::imread(StringValueCStr(filename), NUM2INT(flags));
	if (tmp.empty()) {
	  rb_raise(rb_eStandardError, "Failed to load image");
	  return Qnil;
	}

	dataptr = new cv::Mat();
	tmp.copyTo(*dataptr);
      }
      catch (cv::Exception& e) {
	delete dataptr;
	Error::raise(e);
      }

      return mat2obj(dataptr, klass);
    }

    /*
     * Loads an image from a file.
     *
     * @overload imread(filename, flags)
     *   @param filename [String] Name of file to be loaded.
     *   @param flags [Integer] Flags specifying the color type of a loaded image:
     *     - CV_LOAD_IMAGE_ANYDEPTH - If set, return 16-bit/32-bit image when the input has the corresponding depth, otherwise convert it to 8-bit.
     *     - CV_LOAD_IMAGE_COLOR - If set, always convert image to the color one
     *     - CV_LOAD_IMAGE_GRAYSCALE - If set, always convert image to the grayscale one
     *     - >0 Return a 3-channel color image.
     *     - \=0 Return a grayscale image.
     *     - <0 Return the loaded image as is (with alpha channel).
     *   @return [Mat] Loaded image
     * @opencv_func cv::imread
     */
    VALUE rb_imread(VALUE self, VALUE filename, VALUE flags) {
      return rb_imread_internal(self, filename, flags, rb_klass);
    }

    VALUE rb_imread_as(VALUE self, VALUE filename, VALUE flags, VALUE klass) {
      return rb_imread_internal(self, filename, flags, klass);
    }

    VALUE rb_imwrite_internal(VALUE filename, VALUE img, VALUE params) {
      std::vector<int> params_value;

      if (!NIL_P(params)) {
	Check_Type(params, T_ARRAY);
	int size = RARRAY_LEN(params);
	for (long i = 0; i < size; i++) {
	  VALUE n = rb_ary_entry(params, i);
	  params_value.push_back(NUM2INT(n));
	}
      }

      cv::Mat* m = obj2mat(img);
      bool ret = false;
      try {
	ret = cv::imwrite(StringValueCStr(filename), *m, params_value);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return ret ? Qtrue : Qfalse;
    }

    /*
     * Saves an image to a specified file.
     * The image format is chosen based on the filename extension.
     *
     * @overload save(filename, params = nil)
     *   @param filename [String] Name of the file
     *   @return [Boolean] Result
     * @opencv_func cv::imwrite
     */
    VALUE rb_save(int argc, VALUE* argv, VALUE self) {
      VALUE filename, params;
      rb_scan_args(argc, argv, "11", &filename, &params);
      return rb_imwrite_internal(filename, self, params);
    }

    VALUE rb_imencode_internal(VALUE ext, VALUE img, VALUE params) {
      cv::Mat* dataptr = obj2mat(img);
      std::vector<uchar> buf;
      std::vector<int> params_vector;

      if (!NIL_P(params)) {
	Check_Type(params, T_ARRAY);
	size_t param_size = RARRAY_LEN(params);
	for (size_t i = 0; i < param_size; i++) {
	  params_vector.push_back(NUM2INT(RARRAY_AREF(params, i)));
	}
      }

      try {
	cv::imencode(StringValueCStr(ext), *dataptr, buf, params_vector);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      const size_t size = buf.size();
      VALUE array = rb_ary_new2(size);
      for (size_t i = 0; i < size; i++) {
	rb_ary_push(array, CHR2FIX(buf[i]));
      }
      return array;
    }

    /*
     * Encodes an image into a memory buffer.
     *
     * @overload imencode(ext, params = nil)
     *   @param ext [String] File extension that defines the output format.
     *   @param params [Array<int>] Format-specific parameters.
     *   @return [Array<Fixnum>] Encoded result
     * @opencv_func cv::imwrite
     */
    VALUE rb_imencode(int argc, VALUE* argv, VALUE self) {
      VALUE ext, params;
      rb_scan_args(argc, argv, "11", &ext, &params);
      return rb_imencode_internal(ext, self, params);
    }

    VALUE rb_imdecode_internal(VALUE self, VALUE buf, VALUE flags, VALUE klass) {
      Check_Type(buf, T_ARRAY);

      const size_t size = RARRAY_LEN(buf);
      std::vector<uchar> data(size);
      for (size_t i = 0; i < size; i++) {
	data[i] = (uchar)(NUM2INT(RARRAY_AREF(buf, i)) & 0xff);
      }

      cv::Mat* dstptr = new cv::Mat();
      try {
	cv::imdecode(data, NUM2INT(flags), dstptr);
      }
      catch (cv::Exception& e) {
	delete dstptr;
	Error::raise(e);
      }

      return mat2obj(dstptr, klass);
    }

    /*
     * Reads an image from a buffer in memory.
     * @overload decode(buf, flags)
     *   @param buf [Array] Input array of bytes
     *   @param flags [Integer] Flags specifying the color type of a decoded image (the same flags as imread)
     *   @return [CvMat] Loaded matrix
     * @opencv_func cvDecodeImageM
     */
    VALUE rb_imdecode(VALUE self, VALUE buf, VALUE flags) {
      return rb_imdecode_internal(self, buf, flags, rb_klass);
    }

    VALUE rb_imdecode_as(VALUE self, VALUE buf, VALUE flags, VALUE klass) {
      return rb_imdecode_internal(self, buf, flags, klass);
    }

    /*
     * Returns number of rows of the matrix.
     *
     * @overload rows
     * @return [Integer] Number of rows of the matrix
     */
    VALUE rb_rows(VALUE self) {
      const cv::Mat* dataptr = obj2mat(self);
      return INT2NUM(dataptr->rows);
    }

    /*
     * Returns number of columns of the matrix.
     *
     * @overload cols
     * @return [Integer] Number of columns of the matrix
     */
    VALUE rb_cols(VALUE self) {
      const cv::Mat* dataptr = obj2mat(self);
      return INT2NUM(dataptr->cols);
    }

    /*
     * Returns number of dimensions of the matrix.
     *
     * @overload dims
     * @return [Integer] Number of dimensions of the matrix
     */
    VALUE rb_dims(VALUE self) {
      const cv::Mat* dataptr = obj2mat(self);
      return INT2NUM(dataptr->dims);
    }

    /*
     * Returns depth of the matrix.
     *
     * @overload depth
     * @return [Integer] Depth of the matrix
     */
    VALUE rb_depth(VALUE self) {
      const cv::Mat* dataptr = obj2mat(self);
      return INT2NUM(dataptr->depth());
    }

    /*
     * Returns number of channels of the matrix.
     *
     * @overload channels
     * @return [Integer] Number of channels of the matrix
     */
    VALUE rb_channels(VALUE self) {
      const cv::Mat* dataptr = obj2mat(self);
      return INT2NUM(dataptr->channels());
    }

    /*
     * @overload to_s
     * @return [String] String representation of the matrix
     */
    VALUE rb_to_s(VALUE self) {
      std::stringstream s;
      cv::Mat* selfptr = obj2mat(self);
      s << *selfptr;

      VALUE param[7];
      param[0] = rb_str_new2("<%s:%dx%d,depth=%s,channels=%d,\n%s>");
      param[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      param[2] = INT2NUM(selfptr->rows);
      param[3] = INT2NUM(selfptr->cols);
      param[4] = INT2NUM(selfptr->depth());
      param[5] = INT2NUM(selfptr->channels());
      param[6] = rb_str_new2(s.str().c_str());

      int n = sizeof(param) / sizeof(param[0]);
      return rb_f_sprintf(n, param);
    }

    /**
     * Returns the specified array element.
     *
     * @overload [](i)
     *   @param i [Integer] Zero-based component of the element index
     * @overload [](i, j)
     *   @param i [Integer] Zero-based component of the element index
     *   @param j [Integer] Zero-based component of the element index
     * @overload [](i, j, k)
     *   @param i [Integer] Zero-based component of the element index
     *   @param j [Integer] Zero-based component of the element index
     *   @param k [Integer] Zero-based component of the element index
     * @overload [](i, j, k, ...)
     *   @param i [Integer] Zero-based component of the element index
     *   @param j [Integer] Zero-based component of the element index
     *   @param k [Integer] Zero-based component of the element index
     *   @return [Scalar] Array element
     * @opencv_func cv::Mat.at
     */
    VALUE rb_aref(VALUE self, VALUE args) {
      int index[CV_MAX_DIM];
      const size_t length = RARRAY_LEN(args);
      for (size_t i = 0; i < length; i++) {
	index[i] = NUM2INT(rb_ary_entry(args, i));
      }
      cv::Mat* selfptr = obj2mat(self);
      cv::Scalar* scalar = NULL;
      try {
	switch (selfptr->depth()) {
	case CV_8U:
	  scalar = new cv::Scalar(selfptr->at< cv::Scalar_<uchar> >(index));
	  break;
	case CV_8S:
	  scalar = new cv::Scalar(selfptr->at< cv::Scalar_<char> >(index));
	  break;
	case CV_16U:
	  scalar = new cv::Scalar(selfptr->at< cv::Scalar_<ushort> >(index));
	  break;
	case CV_16S:
	  scalar = new cv::Scalar(selfptr->at< cv::Scalar_<short> >(index));
	  break;
	case CV_32F:
	  scalar = new cv::Scalar(selfptr->at< cv::Scalar_<float> >(index));
	  break;
	case CV_32S:
	  scalar = new cv::Scalar(selfptr->at< cv::Scalar_<int> >(index));
	  break;
	case CV_64F:
	  scalar = new cv::Scalar(selfptr->at< cv::Scalar_<double> >(index));
	  break;
	default:
	  rb_raise(rb_eStandardError, "Invalid depth: %d", selfptr->depth());
	  break;
	}
      }
      catch (cv::Exception& e) {
	delete scalar;
	Error::raise(e);
      }
      return Scalar::scalar2obj(scalar);
    }

    /*
     * Changes the particular array element
     *
     * @overload []=(row, col, value)
     *   @param row [Integer] Row
     *   @param col [Integer] Column
     *   @param value [Scalar] The assigned value
     * @return [Mat] <tt>self</tt>
     * @opencv_func cv::Mat::at
     */
    VALUE rb_aset(VALUE self, VALUE row, VALUE col, VALUE value) {
      cv::Scalar* scalar = Scalar::obj2scalar(value);
      cv::Mat* selfptr = obj2mat(self);
      int y = NUM2INT(row);
      int x = NUM2INT(col) * selfptr->channels();
      try {
	switch (selfptr->depth()) {
	case CV_8U: {
	  uchar* p = &(selfptr->ptr<uchar>(y)[x]);
	  for (int i = 0; i < selfptr->channels(); i++) {
	    p[i] = (uchar)((*scalar)[i]);
	  }
	  break;
	}
	case CV_8S: {
	  char* p = &(selfptr->ptr<char>(y)[x]);
	  for (int i = 0; i < selfptr->channels(); i++) {
	    p[i] = (char)((*scalar)[i]);
	  }
	  break;
	}
	case CV_16U: {
	  ushort* p = &(selfptr->ptr<ushort>(y)[x]);
	  for (int i = 0; i < selfptr->channels(); i++) {
	    p[i] = (ushort)((*scalar)[i]);
	  }
	  break;
	}
	case CV_16S: {
	  short* p = &(selfptr->ptr<short>(y)[x]);
	  for (int i = 0; i < selfptr->channels(); i++) {
	    p[i] = (short)((*scalar)[i]);
	  }
	  break;
	}
	case CV_32F: {
	  float* p = &(selfptr->ptr<float>(y)[x]);
	  for (int i = 0; i < selfptr->channels(); i++) {
	    p[i] = (float)((*scalar)[i]);
	  }
	  break;
	}
	case CV_32S: {
	  int* p = &(selfptr->ptr<int>(y)[x]);
	  for (int i = 0; i < selfptr->channels(); i++) {
	    p[i] = (int)((*scalar)[i]);
	  }
	  break;
	}
	case CV_64F: {
	  double* p = &(selfptr->ptr<double>(y)[x]);
	  for (int i = 0; i < selfptr->channels(); i++) {
	    p[i] = (double)((*scalar)[i]);
	  }
	  break;
	}
	default:
	  rb_raise(rb_eStandardError, "Invalid depth: %d", selfptr->depth());
	  break;
	}
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    /*
     * Computes the per-element sum of two arrays or an array and a scalar.
     *
     * @overload +(value)
     *   @param value [Mat, Scalar] Array or scalar to add
     *   @return [Mat] Result array
     * @opencv_func cv::Mat::operator+
     */
    VALUE rb_add(VALUE self, VALUE other) {
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* retptr = new cv::Mat();
      cv::Mat tmp;

      try {
	if (rb_obj_is_kind_of(other, rb_klass)) {
	  cv::Mat* mat = obj2mat(other);
	  tmp = (*selfptr) + (*mat);
	}
	else if (rb_obj_is_kind_of(other, Scalar::klass())) {
	  cv::Scalar* s = Scalar::obj2scalar(other);
	  tmp = (*selfptr) + (*s);
	}
	else {
	  double scale = NUM2DBL(other);
	  tmp = (*selfptr) + scale;
	}
	tmp.copyTo(*retptr);
      }
      catch (cv::Exception& e) {
	delete retptr;
	Error::raise(e);
      }

      return mat2obj(retptr, CLASS_OF(self));
    }

    /*
     * Computes the per-element difference of two arrays or an array and a scalar.
     *
     * @overload -(value)
     *   @param value [Mat, Scalar] Array or scalar to subtract
     *   @return [Mat] Result array
     * @opencv_func cv::Mat::operator-
     */
    VALUE rb_sub(VALUE self, VALUE other) {
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* retptr = new cv::Mat();
      cv::Mat tmp;

      try {
	if (rb_obj_is_kind_of(other, rb_klass)) {
	  cv::Mat* mat = obj2mat(other);
	  tmp = (*selfptr) - (*mat);
	}
	else if (rb_obj_is_kind_of(other, Scalar::klass())) {
	  cv::Scalar* s = Scalar::obj2scalar(other);
	  tmp = (*selfptr) - (*s);
	}
	else {
	  double scale = NUM2DBL(other);
	  tmp = (*selfptr) - scale;
	}
	tmp.copyTo(*retptr);
      }
      catch (cv::Exception& e) {
	delete retptr;
	Error::raise(e);
      }

      return mat2obj(retptr, CLASS_OF(self));
    }

    /*
     * Computes the per-element product of two arrays or an array and a scalar.
     *
     * @overload -(value)
     *   @param value [Mat, Scalar] Array or scalar to multiply
     *   @return [Mat] Result array
     * @opencv_func cv::Mat::operator*
     */
    VALUE rb_mul(VALUE self, VALUE other) {
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* retptr = new cv::Mat();
      cv::Mat tmp;

      try {
	if (rb_obj_is_kind_of(other, rb_klass)) {
	  cv::Mat* mat = obj2mat(other);
	  tmp = (*selfptr) * (*mat);
	}
	else {
	  double scale = NUM2DBL(other);
	  tmp = (*selfptr) * scale;
	}
	tmp.copyTo(*retptr);
      }
      catch (cv::Exception& e) {
	delete retptr;
	Error::raise(e);
      }

      return mat2obj(retptr, CLASS_OF(self));
    }

    /*
     * Computes the per-element division of two arrays or an array and a scalar.
     *
     * @overload /(value)
     *   @param value [Mat, Scalar] Array or scalar to divide
     *   @return [Mat] Result array
     * @opencv_func cv::Mat::operator/
     */
    VALUE rb_div(VALUE self, VALUE other) {
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* retptr = new cv::Mat();
      cv::Mat tmp;

      try {
	if (rb_obj_is_kind_of(other, rb_klass)) {
	  cv::Mat* mat = obj2mat(other);
	  tmp = (*selfptr) / (*mat);
	}
	else {
	  double scale = NUM2DBL(other);
	  tmp = (*selfptr) / scale;
	}
	tmp.copyTo(*retptr);
      }
      catch (cv::Exception& e) {
	delete retptr;
	Error::raise(e);
      }

      return mat2obj(retptr, CLASS_OF(self));
    }

    VALUE rb_diag(int argc, VALUE *argv, VALUE self) {
      VALUE d;
      rb_scan_args(argc, argv, "01", &d);
      int d_value = NIL_P(d) ? 0 : NUM2INT(d);
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* retptr = NULL;

      try {
	retptr = new cv::Mat();
	cv::Mat tmp = selfptr->diag(d_value);
	tmp.copyTo(*retptr);
      }
      catch (cv::Exception& e) {
	delete retptr;
	Error::raise(e);
      }

      return mat2obj(retptr, CLASS_OF(self));
    }

    /*
     * Computes a dot-product of two vectors.
     *
     * @overload dot(value)
     *   @param value [Mat] Another dot-product operand.
     *   @return [Number] Dot product
     * @opencv_func cv::Mat::dot
     */
    VALUE rb_dot(VALUE self, VALUE other) {
      cv::Mat* selfptr = obj2mat(self);
      VALUE ret = Qnil;

      try {
	cv::Mat* mat = obj2mat(other);
	ret = DBL2NUM(selfptr->dot(*mat));
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return ret;
    }

    /*
     * Sets all or some of the array elements to the specified value.
     *
     * @overload set_to(value, mask = nil)
     *   @param value [Scalar] Assigned scalar converted to the actual array type.
     *   @param mask [Mat] Operation mask of the same size as <tt>self</tt>.
     * @return [Mat] Output array
     */
    VALUE rb_set_to(int argc, VALUE *argv, VALUE self) {
      VALUE value, mask;
      rb_scan_args(argc, argv, "11", &value, &mask);
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* dstptr = new cv::Mat();
      try {
	cv::Mat tmp;
	cv::Scalar* s = Scalar::obj2scalar(value);
	if (NIL_P(mask)) {
	  tmp = selfptr->setTo(*s);
	}
	else {
	  cv::Mat* maskptr = obj2mat(mask);
	  tmp = selfptr->setTo(*s, *maskptr);
	}
	tmp.copyTo(*dstptr);
      }
      catch (cv::Exception& e) {
	delete dstptr;
	Error::raise(e);
      }

      return mat2obj(dstptr, CLASS_OF(self));
    }

    /*
     * Computes the weighted sum of two arrays.
     * This function calculates the weighted sum of two arrays as follows:
     *   dst(I) = src1(I) * alpha + src2(I) * beta + gamma
     *
     * @overload add_weighted(src1, alpha, src2, beta, gamma, dtype = -1)
     *   @param src1 [Mat] The first source array.
     *   @param alpha [Number] Weight for the first array elements.
     *   @param src2 [Mat] The second source array.
     *   @param beta [Number] Weight for the second array elements.
     *   @param gamma [Number] Scalar added to each sum.
     *   @param dtype [Number] optional depth of the output array; when both input arrays have the same depth,
     *     dtype can be set to -1, which will be equivalent to src1.depth.
     * @return [Mat] Output array
     * @opencv_func cv::addWeighted
     */
    VALUE rb_add_weighted(int argc, VALUE *argv, VALUE self) {
      VALUE src1, alpha, src2, beta, gamma, dtype;
      rb_scan_args(argc, argv, "51", &src1, &alpha, &src2, &beta, &gamma, &dtype);
      int dtype_value = NIL_P(dtype) ? -1 : NUM2INT(dtype);

      cv::Mat* src1ptr = obj2mat(src1);
      cv::Mat* src2ptr = obj2mat(src2);
      cv::Mat* dstptr = new cv::Mat();
      try {
	cv::addWeighted(*src1ptr, NUM2DBL(alpha), *src2ptr, NUM2DBL(beta), NUM2DBL(gamma), *dstptr, dtype_value);
      }
      catch (cv::Exception& e) {
	delete dstptr;
	Error::raise(e);
      }

      return mat2obj(dstptr, CLASS_OF(src1));
    }

    /*
     * Scales, computes absolute values, and converts the result to 8-bit.
     *
     * @overload convert_scale_abs(alpha = 1, beta = 0)
     * @param alpha [Number] Optional scale factor.
     * @param beta [Number] Optional delta added to the scaled values.
     * @return [Mat] Output array
     * @opencv_func cv::convertScaleAbs
     */
    VALUE rb_convert_scale_abs(int argc, VALUE *argv, VALUE self) {
      VALUE alpha, beta;
      rb_scan_args(argc, argv, "02", &alpha, &beta);
      double alpha_value = NIL_P(alpha) ? 1.0 : NUM2DBL(alpha);
      double beta_value = NIL_P(beta) ? 0 : NUM2DBL(beta);

      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* dstptr = new cv::Mat();
      try {
	cv::convertScaleAbs(*selfptr, *dstptr, alpha_value, beta_value);
      }
      catch (cv::Exception& e) {
	delete dstptr;
	Error::raise(e);
      }

      return mat2obj(dstptr, CLASS_OF(self));
    }

    /*
     * Converts an array to another data type with optional scaling.
     *
     * @overload convert_to(rtype, alpha = 1, beta = 0)
     * @param rtype [Integer] Desired output matrix type or, rather, the depth
     *   since the number of channels are the same as the input has;
     *   if rtype is negative, the output matrix will have the same type as the input.
     * @param alpha [Number] Optional scale factor.
     * @param beta [Number] Optional delta added to the scaled values.
     * @return [Mat] Output array
     * @opencv_func cv::convertTo
     */
    VALUE rb_convert_to(int argc, VALUE *argv, VALUE self) {
      VALUE rtype, alpha, beta;
      rb_scan_args(argc, argv, "12", &rtype, &alpha, &beta);
      double alpha_value = NIL_P(alpha) ? 1.0 : NUM2DBL(alpha);
      double beta_value = NIL_P(beta) ? 0 : NUM2DBL(beta);

      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* dstptr = new cv::Mat();
      try {
	selfptr->convertTo(*dstptr, NUM2INT(rtype), alpha_value, beta_value);
      }
      catch (cv::Exception& e) {
	delete dstptr;
	Error::raise(e);
      }

      return mat2obj(dstptr, CLASS_OF(self));
    }

    void init() {
      VALUE opencv = rb_define_module("Cv");

      rb_klass = rb_define_class_under(opencv, "Mat", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);

      rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_singleton_method(rb_klass, "zeros", RUBY_METHOD_FUNC(rb_zeros), 3);
      rb_define_singleton_method(rb_klass, "ones", RUBY_METHOD_FUNC(rb_ones), 3);
      rb_define_singleton_method(rb_klass, "eye", RUBY_METHOD_FUNC(rb_eye), 3);

      rb_define_method(rb_klass, "+", RUBY_METHOD_FUNC(rb_add), 1);
      rb_define_method(rb_klass, "-", RUBY_METHOD_FUNC(rb_sub), 1);
      rb_define_method(rb_klass, "*", RUBY_METHOD_FUNC(rb_mul), 1);
      rb_define_method(rb_klass, "/", RUBY_METHOD_FUNC(rb_div), 1);
      rb_define_method(rb_klass, "diag", RUBY_METHOD_FUNC(rb_diag), -1);
      rb_define_method(rb_klass, "dot", RUBY_METHOD_FUNC(rb_dot), 1);

      rb_define_method(rb_klass, "clone", RUBY_METHOD_FUNC(rb_clone), 0);

      rb_define_method(rb_klass, "rows", RUBY_METHOD_FUNC(rb_rows), 0);
      rb_define_alias(rb_klass, "height", "rows");
      rb_define_method(rb_klass, "cols", RUBY_METHOD_FUNC(rb_cols), 0);
      rb_define_alias(rb_klass, "width", "cols");
      
      rb_define_method(rb_klass, "dims", RUBY_METHOD_FUNC(rb_dims), 0);
      rb_define_method(rb_klass, "depth", RUBY_METHOD_FUNC(rb_depth), 0);
      rb_define_method(rb_klass, "channels", RUBY_METHOD_FUNC(rb_channels), 0);

      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), -2);
      rb_define_alias(rb_klass, "at", "[]");
      rb_define_method(rb_klass, "[]=", RUBY_METHOD_FUNC(rb_aset), 3);
      rb_define_method(rb_klass, "set_to", RUBY_METHOD_FUNC(rb_set_to), -1);

      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);

      rb_define_method(rb_klass, "sobel", RUBY_METHOD_FUNC(rb_sobel), -1); // in ext/opencv/mat_imgproc.cpp
      rb_define_method(rb_klass, "canny", RUBY_METHOD_FUNC(rb_canny), -1); // in ext/opencv/mat_imgproc.cpp
      rb_define_method(rb_klass, "laplacian", RUBY_METHOD_FUNC(rb_laplacian), -1); // in ext/opencv/mat_imgproc.cpp

      rb_define_method(rb_klass, "line", RUBY_METHOD_FUNC(rb_line), -1); // in ext/opencv/mat_drawing.cpp
      rb_define_method(rb_klass, "line!", RUBY_METHOD_FUNC(rb_line_bang), -1); // in ext/opencv/mat_drawing.cpp

      rb_define_method(rb_klass, "rectangle", RUBY_METHOD_FUNC(rb_rectangle), -1); // in ext/opencv/mat_drawing.cpp
      rb_define_method(rb_klass, "rectangle!", RUBY_METHOD_FUNC(rb_rectangle_bang), -1); // in ext/opencv/mat_drawing.cpp

      rb_define_method(rb_klass, "resize", RUBY_METHOD_FUNC(rb_resize), -1); // in ext/opencv/mat_imgproc.cpp
      rb_define_method(rb_klass, "cvt_color", RUBY_METHOD_FUNC(rb_cvt_color), -1); // in ext/opencv/mat_imgproc.cpp
      rb_define_method(rb_klass, "blur", RUBY_METHOD_FUNC(rb_blur), -1); // in ext/opencv/mat_imgproc.cpp
      rb_define_method(rb_klass, "gaussian_blur", RUBY_METHOD_FUNC(rb_gaussian_blur), -1); // in ext/opencv/mat_imgproc.cpp

      rb_define_method(rb_klass, "save", RUBY_METHOD_FUNC(rb_save), -1);

      rb_define_method(rb_klass, "imencode", RUBY_METHOD_FUNC(rb_imencode), -1);
      rb_define_singleton_method(rb_klass, "imread_as", RUBY_METHOD_FUNC(rb_imread_as), 3);
      rb_define_singleton_method(rb_klass, "imdecode_as", RUBY_METHOD_FUNC(rb_imdecode_as), 3);

      rb_define_method(rb_klass, "convert_scale_abs", RUBY_METHOD_FUNC(rb_convert_scale_abs), -1);
      rb_define_method(rb_klass, "convert_to", RUBY_METHOD_FUNC(rb_convert_to), -1);
    }
  }
}
