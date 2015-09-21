// ----------------------------------------------------------------
// The contents of this file are distributed under the CC0 license.
// See http://creativecommons.org/publicdomain/zero/1.0/
// ----------------------------------------------------------------

#include "matrix.h"
#include "rand.h"
#include "error.h"
#include "string.h"
#include <fstream>
#include <stdlib.h>
#include <algorithm>

using std::string;
using std::ifstream;
using std::map;
using std::vector;

Matrix::Matrix(const Matrix& other)
{
	throw Ex("Big objects should generally be passed by reference, not by value");
}


void Matrix::setSize(size_t rows, size_t cols)
{
	// Make space for the data
	m_data.resize(rows);
	for(size_t i = 0; i < rows; i++)
		m_data[i].resize(cols);

	// Set the meta-data
	m_filename = "";
	m_attr_name.resize(cols);
	m_str_to_enum.resize(cols);
	m_enum_to_str.resize(cols);
	for(size_t i = 0; i < cols; i++)
	{
		m_str_to_enum[i].clear();
		m_enum_to_str[i].clear();
	}
}

void Matrix::copyMetaData(const Matrix& that)
{
	m_data.clear();
	m_attr_name = that.m_attr_name;
	m_str_to_enum = that.m_str_to_enum;
	m_enum_to_str = that.m_enum_to_str;
}

void Matrix::newColumn(size_t vals)
{
	m_data.clear();
	size_t c = cols();
	string name = "col_";
	name += to_str(c);
	m_attr_name.push_back(name);
	map <string, size_t> temp_str_to_enum;
	map <size_t, string> temp_enum_to_str;
	for(size_t i = 0; i < vals; i++)
	{
		string sVal = "val_";
		sVal += to_str(i);
		temp_str_to_enum[sVal] = i;
		temp_enum_to_str[i] = sVal;
	}
	m_str_to_enum.push_back(temp_str_to_enum);
	m_enum_to_str.push_back(temp_enum_to_str);
}

std::vector<double>& Matrix::newRow()
{
	size_t c = cols();
	if(c == 0)
		throw Ex("You must add some columns before you add any rows.");
	size_t rc = rows();
	m_data.resize(rc + 1);
	std::vector<double>& newrow = m_data[rc];
	newrow.resize(c);
	return newrow;
}

void Matrix::newRows(size_t n)
{
	for(size_t i = 0; i < n; i++)
		newRow();
}

double Matrix::columnMean(size_t col) const
{
	double sum = 0.0;
	size_t count = 0;
	std::vector< std::vector<double> >::const_iterator it;
	for(it = m_data.begin(); it != m_data.end(); it++)
	{
		double val = (*it)[col];
		if(val != UNKNOWN_VALUE)
		{
			sum += val;
			count++;
		}
	}
	return sum / count;
}

double Matrix::columnMin(size_t col) const
{
	double m = 1e300;
	std::vector< std::vector<double> >::const_iterator it;
	for(it = m_data.begin(); it != m_data.end(); it++)
	{
		double val = (*it)[col];
		if(val != UNKNOWN_VALUE)
			m = std::min(m, val);
	}
	return m;
}

double Matrix::columnMax(size_t col) const
{
	double m = -1e300;
	std::vector< std::vector<double> >::const_iterator it;
	for(it = m_data.begin(); it != m_data.end(); it++)
	{
		double val = (*it)[col];
		if(val != UNKNOWN_VALUE)
			m = std::max(m, val);
	}
	return m;
}

double Matrix::mostCommonValue(size_t col) const
{
	map<double, size_t> counts;
	vector< vector<double> >::const_iterator it;
	for(it = m_data.begin(); it != m_data.end(); it++)
	{
		double val = (*it)[col];
		if(val != UNKNOWN_VALUE)
		{
			map<double, size_t>::iterator pair = counts.find(val);
			if(pair == counts.end())
				counts[val] = 1;
			else
				pair->second++;
		}
	}
	size_t valueCount = 0;
	double value = 0;
	for(map<double, size_t>::iterator i = counts.begin(); i != counts.end(); i++)
	{
		if(i->second > valueCount)
		{
			value = i->first;
			valueCount = i->second;
		}
	}
	return value;
}

void Matrix::copyPart(const Matrix& that, size_t rowBegin, size_t colBegin, size_t rowCount, size_t colCount)
{
	if(rowBegin + rowCount > that.rows() || colBegin + colCount > that.cols())
		throw Ex("out of range");

	// Copy the specified region of meta-data
	if(cols() != colCount)
		setSize(0, colCount);
	for(size_t i = 0; i < colCount; i++)
	{
		m_attr_name[i] = that.m_attr_name[colBegin + i];
		m_str_to_enum[i] = that.m_str_to_enum[colBegin + i];
		m_enum_to_str[i] = that.m_enum_to_str[colBegin + i];
	}

	// Copy the specified region of data
	size_t rowsBefore = m_data.size();
	m_data.resize(rowsBefore + rowCount);
	for(size_t i = 0; i < rowCount; i++)
	{
		vector<double>::const_iterator itIn = that[rowBegin + i].begin() + colBegin;
		m_data[rowsBefore + i].resize(colCount);
		vector<double>::iterator itOut = m_data[rowsBefore + i].begin();
		for(size_t j = 0; j < colCount; j++)
			*itOut++ = *itIn++;
	}
}

string toLower(string strToConvert)
{
	//change each element of the string to lower case
	for(size_t i = 0; i < strToConvert.length(); i++)
		strToConvert[i] = tolower(strToConvert[i]);
	return strToConvert;//return the converted string
}

void Matrix::saveARFF(string filename) const
{
	std::ofstream s;
	s.exceptions(std::ios::failbit|std::ios::badbit);
	try
	{
		s.open(filename.c_str(), std::ios::binary);
	}
	catch(const std::exception&)
	{
		throw Ex("Error creating file: ", filename);
	}
	s.precision(10);
	s << "@RELATION " << m_filename << "\n";
	for(size_t i = 0; i < m_attr_name.size(); i++)
	{
		s << "@ATTRIBUTE " << m_attr_name[i];
		if(m_attr_name[i].size() == 0)
			s << "x";
		size_t vals = valueCount(i);
		if(vals == 0)
			s << " REAL\n";
		else
		{
			s << " {";
			for(size_t j = 0; j < vals; j++)
			{
				s << attrValue(i, j);
				if(j + 1 < vals)
					s << ",";
			}
			s << "}\n";
		}
	}
	s << "@DATA\n";
	for(size_t i = 0; i < rows(); i++)
	{
		const std::vector<double>& r = (*this)[i];
		for(size_t j = 0; j < cols(); j++)
		{
			if(r[j] == UNKNOWN_VALUE)
				s << "?";
			else
			{
				size_t vals = valueCount(j);
				if(vals == 0)
					s << to_str(r[j]);
				else
				{
					size_t val = (size_t)r[j];
					if(val >= vals)
						throw Ex("value out of range");
					s << attrValue(j, val);
				}
			}
			if(j + 1 < cols())
				s << ",";
		}
		s << "\n";
	}
}

void Matrix::loadARFF(string fileName)
{
	size_t lineNum = 0;
	string line;                 //line of input from the arff file
	ifstream inputFile;          //input stream
	map <string, size_t> tempMap;   //temp map for int->string map (attrInts)
	map <size_t, string> tempMapS;  //temp map for string->int map (attrString)
	size_t attrCount = 0;           //Count number of attributes

	inputFile.open ( fileName.c_str() );
	if ( !inputFile )
		throw Ex ( "failed to open the file: ", fileName );
	while ( !inputFile.eof() && inputFile )
	{
		//Iterate through each line of the file
		getline ( inputFile, line );
		lineNum++;
		if ( toLower ( line ).find ( "@relation" ) == 0 )
			m_filename = line.substr ( line.find_first_of ( " " ) );
		else if ( toLower ( line ).find ( "@attribute" ) == 0 )
		{
			line = line.substr ( line.find_first_of ( " \t" ) + 1 );
			
			string attrName;
			
			// If the attribute name is delimited by ''
			if ( line.find_first_of( "'" ) == 0 )
			{
				attrName = line.substr ( 1 );
				attrName = attrName.substr ( 0, attrName.find_first_of( "'" ) );
				line = line.substr ( attrName.size() + 2 );
			}
			else
			{
				attrName = line.substr( 0, line.find_first_of( " \t" ) );
				line = line.substr ( attrName.size() );
			}
			
			m_attr_name.push_back ( attrName );
			
			string value = line.substr ( line.find_first_not_of ( " \t" ) );
			
			tempMap.clear();
			tempMapS.clear();

			//If the attribute is nominal
			if ( value.find_first_of ( "{" ) == 0 )
			{
				size_t firstComma;
				size_t firstSpace;
				size_t firstLetter;
				value = value.substr ( 1, value.find_last_of ( "}" ) - 1 );
				size_t valCount = 0;
				string tempValue;

				//Parse the attributes--push onto the maps
				while ( ( firstComma = value.find_first_of ( "," ) ) != string::npos )
				{
					firstLetter = value.find_first_not_of ( " \t," );

					value = value.substr ( firstLetter );
					firstComma = value.find_first_of ( "," );
					firstSpace = value.find_first_of ( " \t" );
					tempMapS[valCount] = value.substr ( 0, firstComma );
					string valName = value.substr ( 0, firstComma );
					valName = valName.substr ( 0, valName.find_last_not_of(" \t") + 1);
					tempMap[valName] = valCount++;
					firstComma = ( firstComma < firstSpace &&
						firstSpace < ( firstComma + 2 ) ) ? firstSpace :
						firstComma;
					value = value.substr ( firstComma + 1 );
				}

				//Push final attribute onto the maps
				firstLetter = value.find_first_not_of ( " \t," );
				value = value.substr ( firstLetter );
				string valName = value.substr ( 0, value.find_last_not_of(" \t") + 1);
				tempMapS[valCount] = valName;
				tempMap[valName] = valCount++;
				m_str_to_enum.push_back ( tempMap );
				m_enum_to_str.push_back ( tempMapS );
			}
			else
			{
				//The attribute is continuous
				m_str_to_enum.push_back ( tempMap );
				m_enum_to_str.push_back ( tempMapS );
			}
			attrCount++;
		}
		else if ( toLower ( line ).find ( "@data" ) == 0 )
		{
			vector<double> temp; //Holds each line of data
			temp.resize(attrCount);
			m_data.clear();
			while ( !inputFile.eof() )
			{
				getline ( inputFile, line );
				lineNum++;
				if(line.length() == 0 || line[0] == '%' || line[0] == '\n' || line[0] == '\r')
					continue;
				size_t pos = 0;
				for ( size_t i = 0; i < attrCount; i++ )
				{
					size_t vals = valueCount ( i );
					size_t valStart = line.find_first_not_of ( " \t", pos );
					if(valStart == string::npos)
						throw Ex("Expected more elements on line ", to_str(lineNum));
					size_t valEnd = line.find_first_of ( ",\n\r", valStart );
					string val;
					if(valEnd == string::npos)
					{
						if(i + 1 == attrCount)
							val = line.substr( valStart );
						else
							throw Ex("Expected more elements on line ", to_str(lineNum));
					}
					else
						val = line.substr ( valStart, valEnd - valStart );
					pos = valEnd + 1;
					if ( vals > 0 ) //if the attribute is nominal...
					{
						if ( val == "?" )
							temp[i] = UNKNOWN_VALUE;
						else
						{
							map<string, size_t>::iterator it = m_str_to_enum[i].find ( val );
							if(it == m_str_to_enum[i].end())
								throw Ex("Unrecognized enumeration value, \"", val, "\" on line ", to_str(lineNum), ", attr ", to_str(i)); 
							temp[i] = (double)m_str_to_enum[i][val];
						}
					}
					else
					{
						// The attribute is continuous
						if ( val == "?" )
							temp[i] = UNKNOWN_VALUE;
						else
							temp[i] = atof( val.c_str() );
					}
				}
				m_data.push_back ( temp );
			}
		}
	}
}

const std::string& Matrix::attrValue(size_t attr, size_t val) const
{
	std::map<size_t, std::string>::const_iterator it = m_enum_to_str[attr].find(val);
	if(it == m_enum_to_str[attr].end())
		throw Ex("no name");
	return it->second;
}

void Matrix::setAll(double val)
{
	size_t c = cols();
	std::vector< std::vector<double> >::iterator it;
	for(it = m_data.begin(); it != m_data.end(); it++)
		it->assign(c, val);
}

void Matrix::checkCompatibility(const Matrix& that) const
{
	size_t c = cols();
	if(that.cols() != c)
		throw Ex("Matrices have different number of columns");
	for(size_t i = 0; i < c; i++)
	{
		if(valueCount(i) != that.valueCount(i))
			throw Ex("Column ", to_str(i), " has mis-matching number of values");
	}
}

