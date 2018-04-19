#include "MStatusException.hpp"
#include <string.h>

wlib::MStatusException::MStatusException(const MStatus & stat, const MString & message, const MString & place)
	: stat(stat), message(message), place(place)
{}

wlib::MStatusException::~MStatusException() {}

MString wlib::MStatusException::toString(void) const {
	return this->toString(place);
}

MString wlib::MStatusException::toString(const MString & place_override) const
{
	//FORMAT | [STAT] PLACE : MESSAGE
	MString stat_str("UNKNOWN");
	switch (stat.statusCode()) {
	case MStatus::MStatusCode::kEndOfFile: stat_str = "EndOfFile"; break;
	case MStatus::MStatusCode::kFailure: stat_str = "Failure"; break;
	case MStatus::MStatusCode::kInsufficientMemory: stat_str = "InsufficientMemory"; break;
	case MStatus::MStatusCode::kInvalidParameter: stat_str = "InvalidParameter"; break;
	case MStatus::MStatusCode::kLicenseFailure: stat_str = "LicenseFailure"; break;
	case MStatus::MStatusCode::kNotFound: stat_str = "NotFound"; break;
	case MStatus::MStatusCode::kNotImplemented: stat_str = "NotImplemented"; break;
	case MStatus::MStatusCode::kSuccess: stat_str = "Success"; break;
	case MStatus::MStatusCode::kUnknownParameter: stat_str = "UnknownParameter"; break;
	}

	return MString("[" + stat_str + "] " + (place == "<unlogged>" ? place_override : place) + " : " + message + "(" + stat.errorString() + ")");
}

wlib::MStatusException::operator MStatus() const
{
	return this->stat;
}

wlib::MStatusException::operator MString() const
{
	return this->toString();
}

bool wlib::MStatusException::operator==(const MStatus & comp) const
{
	return (this->stat == comp);
}

void wlib::MStatusException::throwIf(const MStatus & stat, const MString & message, const MString & place) {
	if (stat.error()) throw MStatusException(stat, message, place);
}

std::ostream & wlib::operator<<(ostream & os, const MStatusException & e)
{
	os << e.toString();
	return os;
}
