// Copyright (c) 2013 Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "version.h"

#include <cstdlib>
#include <sstream>
#include <string>

Version::Version()
	: major_(0)
	, minor_(0)
	, patch_(0)
{
}

Version::Version(const std::string &string)
	: major_(0)
	, minor_(0)
	, patch_(0)
{
	FromString(string);
}

Version::Version(int major, int minor, int patch)
	: major_(major)
	, minor_(minor)
	, patch_(patch)
{
}

void Version::FromString(const std::string &string) {
	std::stringstream stream;
	stream << string;

	std::string component;
	if (stream) {
		std::getline(stream, component, '.');
		major_ = std::atoi(component.c_str());
		component.clear();
	}
	if (stream) {
		std::getline(stream, component, '.');
		minor_ = std::atoi(component.c_str());
		component.clear();
	}
	if (stream) {
		std::getline(stream, component, '.');
		patch_ = std::atoi(component.c_str());
		component.clear();
	}
}

std::string Version::AsString(bool include_trailing_zero) const {
	std::stringstream stream;
	stream << major_ << '.' << minor_;
	if (patch_ != 0 || (patch_ == 0 && include_trailing_zero)) {
		stream << '.' << patch_;
	}
	return stream.str();
}

bool Version::operator<(const Version &rhs) const {
	if (major_ < rhs.major_) {
		return true;
	} else if (major_ == rhs.major_) {
		if (minor_ < rhs.minor_) {
			return true;
		} else if (minor_ == rhs.minor_) {
			if (patch_ < rhs.patch_) {
				return true;
			}
		}
	}
	return false;
}

bool Version::operator==(const Version &rhs) const {
	return major_ == rhs.major_ &&
		   minor_ == rhs.minor_ &&
		   patch_ == rhs.patch_;
}

bool Version::operator!=(const Version &rhs) const {
	return !(*this == rhs);
}