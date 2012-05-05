/* -*- c++ -*- */
/*
 * Copyright 2012 Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <fstream>
#include <string>
#include <sstream>

#include <boost/assign.hpp>

#include <gr_io_signature.h>
#include <gr_deinterleave.h>
#include <gr_float_to_complex.h>

#include "rtl_tcp_source_c.h"

#include <osmosdr_arg_helpers.h>

using namespace boost::assign;

rtl_tcp_source_c_sptr make_rtl_tcp_source_c(const std::string &args)
{
  return gnuradio::get_initial_sptr(new rtl_tcp_source_c(args));
}

rtl_tcp_source_c::rtl_tcp_source_c(const std::string &args) :
  gr_hier_block2("rtl_tcp_source_c",
                 gr_make_io_signature (0, 0, 0),
                 gr_make_io_signature (1, 1, sizeof (gr_complex)))
{
  std::string host = "127.0.0.1";
  unsigned short port = 1234;
  bool eof = false;
  bool wait = true;
  int payload_size = 16384;

  _freq = 0;
  _rate = 0;

  dict_t dict = params_to_dict(args);

  if (dict.count("host"))
    host = dict["host"];

  if (dict.count("port"))
    port = boost::lexical_cast< unsigned short >( dict["port"] );

  if (dict.count("eof"))
    eof = "true" == dict["eof"] ? true : false;

  if (dict.count("wait"))
    wait = "true" == dict["wait"] ? true : false;

  if (dict.count("psize"))
    payload_size = boost::lexical_cast< int >( dict["psize"] );

  if (!host.length())
    host = "127.0.0.1";

  if (0 == port)
    port = 1234;

  if (payload_size <= 0)
    payload_size = 16384;

  _src = make_rtl_tcp_source_f(sizeof(float), host.c_str(), port, payload_size,
                               eof, wait);

  /* rtl tcp source provides a stream of interleaved IQ floats */
  gr_deinterleave_sptr deinterleave = gr_make_deinterleave(sizeof(float));

  /* block to convert deinterleaved floats to a complex stream */
  gr_float_to_complex_sptr f2c = gr_make_float_to_complex(1);

  connect(_src, 0, deinterleave, 0);
  connect(deinterleave, 0, f2c, 0); /* I */
  connect(deinterleave, 1, f2c, 1); /* Q */
  connect(f2c, 0, self(), 0);
}

rtl_tcp_source_c::~rtl_tcp_source_c()
{
}

gr_basic_block_sptr rtl_tcp_source_c::self()
{
  return gr_hier_block2::self();
}

std::string rtl_tcp_source_c::name()
{
  return "RTL TCP Client Source";
}

size_t rtl_tcp_source_c::get_num_channels( void )
{
  return 1;
}

osmosdr::meta_range_t rtl_tcp_source_c::get_sample_rates( void )
{
  osmosdr::meta_range_t range;

  range += osmosdr::range_t( get_sample_rate() );

  return range;
}

double rtl_tcp_source_c::set_sample_rate( double rate )
{
  return get_sample_rate();
}

double rtl_tcp_source_c::get_sample_rate( void )
{
  return _rate;
}

osmosdr::freq_range_t rtl_tcp_source_c::get_freq_range( size_t chan )
{
  osmosdr::freq_range_t range(50e6, 2.2e6, 100);

  return range;
}

double rtl_tcp_source_c::set_center_freq( double freq, size_t chan )
{
  _src->set_freq( int(freq) );

  _freq = freq;

  return get_center_freq(chan);
}

double rtl_tcp_source_c::get_center_freq( size_t chan )
{
  return _freq;
}

double rtl_tcp_source_c::set_freq_corr( double ppm, size_t chan )
{
  return get_freq_corr( chan );
}

double rtl_tcp_source_c::get_freq_corr( size_t chan )
{
  return 0;
}

std::vector<std::string> rtl_tcp_source_c::get_gain_names( size_t chan )
{
  return std::vector< std::string >();
}

osmosdr::gain_range_t rtl_tcp_source_c::get_gain_range( size_t chan )
{
  osmosdr::gain_range_t range(0, 0);

  return range;
}

osmosdr::gain_range_t rtl_tcp_source_c::get_gain_range( const std::string & name, size_t chan )
{
  return get_gain_range( chan );
}

bool rtl_tcp_source_c::set_gain_mode( bool mode, size_t chan )
{
  return get_gain_mode(chan);
}

bool rtl_tcp_source_c::get_gain_mode( size_t chan )
{
  return true;
}

double rtl_tcp_source_c::set_gain( double gain, size_t chan )
{
  return get_gain(chan);
}

double rtl_tcp_source_c::set_gain( double gain, const std::string & name, size_t chan )
{
  return set_gain(chan);
}

double rtl_tcp_source_c::get_gain( size_t chan )
{
  return 0;
}

double rtl_tcp_source_c::get_gain( const std::string & name, size_t chan )
{
  return get_gain(chan);
}

std::vector< std::string > rtl_tcp_source_c::get_antennas( size_t chan )
{
  std::vector< std::string > antennas;

  antennas += get_antenna(chan);

  return antennas;
}

std::string rtl_tcp_source_c::set_antenna( const std::string & antenna, size_t chan )
{
  return get_antenna(chan);
}

std::string rtl_tcp_source_c::get_antenna( size_t chan )
{
  return "ANT";
}