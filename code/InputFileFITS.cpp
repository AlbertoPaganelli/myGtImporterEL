/***************************************************************************
    begin                : Jul 18 2013
    copyright            : (C) 2013 Andrea Zoli
    email                : zoli@iasfbo.inaf.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software for non commercial purpose              *
 *   and for public research institutes; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License.          *
 *   For commercial purpose see appropriate license terms                  *
 *                                                                         *
 ***************************************************************************/

#include "Definitions.h"
#include "InputFileFITS.h"
#include <cstring>

namespace qlbase {

#define ERRMSGSIZ 81

InputFileFITS::InputFileFITS() : opened(false), infptr(0) {
}

InputFileFITS::~InputFileFITS() {
}

void InputFileFITS::throwException(const char *msg, int status) {
	std::string errMsg(msg);

	if(status != 0)
	{
		char errDesc[ERRMSGSIZ];
		fits_read_errmsg(errDesc);
		errMsg += errDesc;
	}
	else
		errMsg += "Reading from a closed file.";

	throw IOException(msg, status);
}

void InputFileFITS::open(const std::string &filename) {
	int status = 0;
	fits_open_table(&infptr, filename.c_str(), READONLY, &status);

	if (status)
		throwException("Error in InputFileFITS::Open() ", status);

	opened = true;
}

void InputFileFITS::close() {
	int status = 0;
	fits_close_file(infptr, &status);

	if (status)
		throwException("Error in InputFileFITS::Close() ", status);

	opened = false;
}

void InputFileFITS::moveToHeader(int number) {
	int status = 0;

	if(!isOpened())
		throwException("Error in InputFileFITS::jumpToChuck() ", status);

	fits_movabs_hdu(infptr, number+1, 0, &status);

	if (status)
		throwException("Error in InputFileFITS::jumpToChuck() ", status);
}

int InputFileFITS::getNCols() {
	int status = 0;

	if(!isOpened())
		throwException("Error in InputFileFITS::getNCols() ", status);

	int ncols;
	fits_get_num_cols(infptr, &ncols, &status);

	if (status)
		throwException("Error in InputFileFITS::getNCols() ", status);

	return ncols;
}

long InputFileFITS::getNRows() {
	int status = 0;

	if(!isOpened())
		throwException("Error in InputFileFITS::getNRows() ", status);

	long nrows;
	fits_get_num_rows(infptr, &nrows, &status);

	if (status)
		throwException("Error in InputFileFITS::getNRows() ", status);

	return nrows;
}

std::vector<uint8_t> InputFileFITS::readu8i(int ncol, long frow, long lrow) {
	std::vector<uint8_t> buff;
	_read(ncol, buff, TBYTE, frow, lrow);
	return buff;
}

std::vector<int16_t> InputFileFITS::read16i(int ncol, long frow, long lrow) {
	std::vector<int16_t> buff;
	_read(ncol, buff, TSHORT, frow, lrow);
	return buff;
}

std::vector<int32_t> InputFileFITS::read32i(int ncol, long frow, long lrow) {
	std::vector<int32_t> buff;
	_read(ncol, buff, TINT, frow, lrow);
	return buff;
}

std::vector<int64_t> InputFileFITS::read64i(int ncol, long frow, long lrow) {
	std::vector<int64_t> buff;
	_read(ncol, buff, TLONG, frow, lrow);
	return buff;
}

std::vector<float> InputFileFITS::read32f(int ncol, long frow, long lrow) {
	std::vector<float> buff;
	_read(ncol, buff, TFLOAT, frow, lrow);
	return buff;
}

std::vector<double> InputFileFITS::read64f(int ncol, long frow, long lrow) {
	std::vector<double> buff;
	_read(ncol, buff, TDOUBLE, frow, lrow);
	return buff;
}

std::vector< std::vector<uint8_t> > InputFileFITS::readu8iv(int ncol, long frow, long lrow, int vsize)
{
	std::vector< std::vector<uint8_t> > buff;
	_readv(ncol, buff, TBYTE, frow, lrow, vsize);
	return buff;
}

std::vector< std::vector<int16_t> > InputFileFITS::read16iv(int ncol, long frow, long lrow, int vsize)
{
	std::vector< std::vector<int16_t> > buff;
	_readv(ncol, buff, TSHORT, frow, lrow, vsize);
	return buff;
}

std::vector< std::vector<int32_t> > InputFileFITS::read32iv(int ncol, long frow, long lrow, int vsize)
{
	std::vector< std::vector<int32_t> > buff;
	_readv(ncol, buff, TINT, frow, lrow, vsize);
	return buff;
}

std::vector< std::vector<int64_t> > InputFileFITS::read64iv(int ncol, long frow, long lrow, int vsize)
{
	std::vector< std::vector<int64_t> > buff;
	_readv(ncol, buff, TLONG, frow, lrow, vsize);
	return buff;
}

std::vector< std::vector<float> > InputFileFITS::read32fv(int ncol, long frow, long lrow, int vsize)
{
	std::vector< std::vector<float> > buff;
	_readv(ncol, buff, TFLOAT, frow, lrow, vsize);
	return buff;
}

std::vector< std::vector<double> > InputFileFITS::read64fv(int ncol, long frow, long lrow, int vsize)
{
	std::vector< std::vector<double> > buff;
	_readv(ncol, buff, TDOUBLE, frow, lrow, vsize);
	return buff;
}

Image<uint8_t> InputFileFITS::readImageu8i()
{
	Image<uint8_t> buff;
	_readImage(buff, TBYTE);
	return buff;
}

Image<int16_t> InputFileFITS::readImage16i()
{
	Image<int16_t> buff;
	_readImage(buff, TSHORT);
	return buff;
}

Image<int32_t> InputFileFITS::readImage32if()
{
	Image<int32_t> buff;
	_readImage(buff, TINT);
	return buff;
}

Image<int64_t> InputFileFITS::readImage64i()
{
	Image<int64_t> buff;
	_readImage(buff, TLONG);
	return buff;
}

Image<float> InputFileFITS::readImage32f()
{
	Image<float> buff;
	_readImage(buff, TFLOAT);
	return buff;
}

Image<double> InputFileFITS::readImage64f()
{
	Image<double> buff;
	_readImage(buff, TDOUBLE);
	return buff;
}

template<class T>
void InputFileFITS::_read(int ncol, std::vector<T>& buff, int type, long frow, long lrow) {
	int status = 0;
	if(!isOpened())
		throwException("Error in InputFileFITS::read() ", status);

	int anynull;
	long nelem = lrow - frow + 1;
	long null = 0;

	buff.resize(nelem);

	fits_read_col(infptr, type, ncol+1, frow+1, 1, nelem, &null,  &buff[0], &anynull, &status);

	if(status)
		throwException("Error in InputFileFITS::read() ", status);
}

template<class T>
void InputFileFITS::_readv(int ncol, std::vector< std::vector<T> >& buff, int type, long frow, long lrow, int vsize) {
	int status = 0;
	if(!isOpened())
		throwException("Error in InputFileFITS::_readv() ", status);

	int anynull;
	long nelem = (lrow - frow + 1);
	long null = 0;

	T* buffptr = new T[nelem*vsize];
	fits_read_col(infptr, type, ncol+1, frow+1, 1, nelem*vsize, &null,  buffptr, &anynull, &status);

	// conversion from row pointers to std::vector< std::vector<T> >
	buff.resize(nelem);
	for(unsigned int i=0; i<nelem; i++)
		buff[i].resize(vsize);

	for(unsigned int row = frow; row < lrow+1; row++)
		memcpy(&buff[row][0], &buffptr[row*vsize], vsize*sizeof(T));

	delete[] buffptr;

	if(status)
		throwException("Error in InputFileFITS::_readv() ", status);
}

template<class T>
void InputFileFITS::_readImage(Image<T>& buff, int type)
{
	int status = 0;
	if(!isOpened())
		throwException("Error in InputFileFITS::readImage() ", status);

	int bitpix;
	int naxis;
	const int MAXDIM = 3;
	long naxes[MAXDIM];
	fits_get_img_param(infptr, MAXDIM,  &bitpix, &naxis, naxes, &status);
	if(!isOpened())
		throwException("Error in InputFileFITS::readImage() ", status);


	long fpixel[MAXDIM];
	long nelements = 1;
	for(int dim=0; dim<naxis; dim++)
	{
		fpixel[dim] = 1;
		nelements *= naxes[dim];
	}

	long nulval = 0;
	int anynul;

	buff.data.resize(nelements);
	fits_read_pix(infptr, type, fpixel, nelements, &nulval, &buff.data[0], &anynul, &status);
	if(!isOpened())
		throwException("Error in InputFileFITS::readImage() ", status);

	buff.dim = naxis;

	buff.sizes.resize(0);
	for(int i=0; i<naxis; i++)
		buff.sizes.push_back(naxes[i]);
}

}
