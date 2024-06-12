#pragma once
/*
	# Point Cloud Map (Library)
	Version 1.1.0
	By Joseph M. Juma

	## About
	A library implementing data models, importing and exporting around the 
	"Point Cloud Map" (*.pcm) file type.
	
	## Usage
	See the `pcm::PointCloudMap` structure for the best usage notes.

	## Copyright
	Copyright Joseph M. Juma, 2024. All rights reserved.
*/
#ifndef POINT_CLOUD_MAP_LIBRARY__H
#define POINT_CLOUD_MAP_LIBRARY__H
/* Deps */
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

/* Metadata */
#define POINT_CLOUD_MAP_LIBRARY_VERSION "1.1.0"

/* Namespace */
namespace pcm
{
	/* Type Definitions */

	typedef uint64_t uint64;

	typedef float real32;
	typedef double real64;

	typedef std::string string;

	typedef std::string ReferenceType;
	typedef std::string ReferenceValue;

	/* Functions */
	
	// String Utility Functions
	#ifndef NOVA_STRING_FUNCS__CPP
	std::vector<string> split_string(string s, string delimiter)
	{
		/*
			Splits a string into separate substrings delimited by the `delim` substring.
			The delim substring can be multiple characters long, in which case it will
			only split where the full substring is present.
		*/

		std::vector<string> strings;
		uint64 pos = 0;
		uint64 next = 0;

		// @todo: improve code safety with exceptions. Notably, a bad_alloc on push_back, or out_of_range!
		while (pos < s.length())
		{
			next = s.find(delimiter, pos);

			// If next isn't found, append the remainder of the string.
			if (next == std::string::npos)
			{
				strings.push_back(s.substr(pos));
				break;
			}
			else
			{
				strings.push_back(s.substr(pos, next - pos));
				pos = next + delimiter.length();
			};
		};

		return strings;
	}
	string replaceAll(string s, string _find, string _replace, int offset = 0, const bool respectQuotes = true)
	{
		/*
			Find each instance of a given value in a stringand replace it. Will
			optionally not run on things in quotes if respectQuotes is true.
		*/

		string tmp = s;
		bool inQuotes = false;
		uint64 quotePos = 0;

		// For each instance of _find that's found,
		uint64 pos = tmp.find(_find, offset);
		while (pos != tmp.npos)
		{
			// GPT Loop for quote checks,
			while (quotePos != tmp.npos && quotePos < pos) {
				quotePos = tmp.find_first_of("\"'", quotePos);
				if (quotePos != tmp.npos && quotePos < pos) {
					inQuotes = !inQuotes;
					quotePos++;
				}
			};

			// If not in quotes, replace it and move the position forward by _replace.length()
			if (!inQuotes)
			{
				tmp.replace(pos, _find.length(), _replace);
				pos = tmp.find(_find, pos + _replace.length());
			}
			// Else (if in quotes) don't replace it, but move the position forward by _find.length(),
			else
			{
				pos = tmp.find(_find, pos + _find.length());
			};
		};
		return tmp;
	}
	inline std::vector<string> split_lines(const string& data)
	{
		// Split into lines on the "\n" character after stripping the "\r" character,
		std::vector<string> lines = split_string(replaceAll(data, "\r\n", "\n"), "\n");

		// Strip out all the "\n" characters,
		for (uint64 i = 0; i < lines.size(); i++)
		{
			lines[i] = replaceAll(lines[i], "\n", "");
		};

		// Return lines
		return lines;
	};
	#endif

	/* Structures */

	// Mathematical Structures
	struct Vector3f
	{
		/*
			# Vector 3D (struct)

			## Description
			A simple 3D vector.
		*/

		/* Elements */
		real32 value[3];

		/* Methods */

		// Constructors & Destructor
		Vector3f()
		{
			this->value[0] = 0.0f;
			this->value[1] = 0.0f;
			this->value[2] = 0.0f;
		};
		Vector3f(const Vector3f& _source)
		{
			this->value[0] = _source.value[0];
			this->value[1] = _source.value[1];
			this->value[2] = _source.value[2];
		};
		Vector3f(const real32& _x, const real32& _y, const real32& _z)
		{
			this->value[0] = _x;
			this->value[1] = _y;
			this->value[2] = _z;
		};

		// Binary Operators
		inline Vector3f operator+(const Vector3f& B) const
		{
			return Vector3f(
				this->value[0] + B.value[0],
				this->value[1] + B.value[1],
				this->value[2] + B.value[2]
			);
		};
		inline Vector3f& operator+=(const Vector3f& B)
		{
			this->value[0] += B.value[0];
			this->value[1] += B.value[1];
			this->value[2] += B.value[2];

			return (*this);
		};
		inline bool operator==(const Vector3f& B) const
		{
			return (
				(this->value[0] == B.value[0]) &&
				(this->value[1] == B.value[1]) &&
				(this->value[2] == B.value[2])
			);
		};

		// Utility Methods
		inline void zero()
		{
			this->value[0] = 0.0f;
			this->value[1] = 0.0f;
			this->value[2] = 0.0f;
		};

		// Serialization
		inline std::string toString() const
		{
			return (
				std::to_string(this->value[0]) + " " +
				std::to_string(this->value[1]) + " " +
				std::to_string(this->value[2])
				);
		};

		// Deserialization
		inline bool fromString(const std::string& data)
		{
			return false; // @todo
		};
	};
	
	// Reference Structures
	struct ReferenceId
	{
		/*
			# Reference Id (struct)

			## Description
			The Id for a given reference, represented as an arbitrary text string.
		*/

		/* Elements */
		std::string value;

		/* Methods */
		// Constructors & Destructor
		ReferenceId(const std::string& v = "")
		{
			this->value = v;
		};

		// Equality Operators
		inline bool operator==(const ReferenceId& B) const
		{
			return (strcmp(this->value.c_str(), B.value.c_str()) == 0);
		};

		// Relational Operators
		inline bool operator<(const ReferenceId& B) const
		{
			return (this->value < B.value);
		};

		// Serialization Methods
		inline std::string toString() const
		{
			return this->value;
		};
	};
	struct Reference
	{
		/*
			# Reference (struct)

			## Description
			A reference which associates an id value, with a value of some type.
		*/

		/* Elements */
		ReferenceType type;
		ReferenceValue value;

		/* Methods */
		
		// Constructors & Destructor
		Reference(const std::string& _type = "", const std::string& _value = "")
		{
			this->type = _type;
			this->value = _value;
		};

		// Serialization
		inline std::string toString() const
		{
			return (this->type + " " + this->value);
		};
	};

	// Point Structures
	struct Point3D
	{
		/*
			# 3D Point (struct)

			## Description
			A point which associates a 3D position with some value data.
		*/

		/* Elements */
		ReferenceId reference;
		Vector3f position;

		/* Methods */
		// Constructors & Destructor
		Point3D()
		{
			this->reference = ReferenceId();
			this->position = Vector3f();
		};
		Point3D(const Point3D& _source)
		{
			this->reference = _source.reference;
			this->position = _source.position;
		};
		Point3D(const ReferenceId& _reference, const Vector3f& _position)
		{
			this->reference = _reference;
			this->position = _position;
		};

		// Serialization
		inline std::string toString() const
		{
			return "point " + this->reference.toString() + " " + this->position.toString();
		};
	};

	// Table Structures
	struct ReferenceTable
	{
		/*
			# Reference Table (struct)

			## Description
			Stores a collection of reference entries.
		*/

		/* Elements */
		std::map<ReferenceId, Reference> values;

		/* Methods */

		// Constructors & Destructor
		ReferenceTable()
		{
			this->values.clear();
		};
		virtual ~ReferenceTable()
		{
			this->values.clear();
		};

		// Clearing
		inline void clear()
		{
			this->values.clear();
		};

		// Utility Methods
		inline bool in(const ReferenceId& id) const
		{
			return (this->values.find(id) != this->values.end());
		};
		inline std::vector<std::pair<ReferenceId, Reference>> getReferenceList()
		{
			/*
				Returns a list of references in pair form. This is primarily provided for 
				external applications to employ this in iterating through the references of 
				a given PCM.
			*/

			std::vector<std::pair<ReferenceId, Reference>> refs;
			for (std::pair<ReferenceId, Reference> ref : this->values)
			{
				refs.push_back(ref);
			};
			return refs;
		};

		// Data Operations
		bool insert(const ReferenceId& id, const Reference& ref)
		{
			// If the given Id isn't already found, insert it
			if (!this->in(id))
			{
				this->values[id] = ref;
			}
			// Else, if id already exists - return error.
			else
			{
				return false;
			};

			return true;
		};

		// Serialization
		inline std::string toString() const
		{
			std::string str = "\n";
			for (auto pair : this->values)
			{
				str += "ref " + pair.first.toString() + " " + pair.second.toString() + "\n";
			};
			str += "\n";
			return str;
		};
	};
	struct PointTable
	{
		/*
			# Point Cloud Table (struct)

			## Description
			Stores a list of 3D points which relate a position in 3D space to reference 
			data.
		*/

		/* Elements */
		std::vector<Point3D*> points;

		/* Methods */

		// Constructors & Destructor
		PointTable()
		{
			this->points.clear();
		};
		virtual ~PointTable()
		{
			this->clear();
		};

		// Clearing
		inline void clear()
		{
			for (uint64_t i = 0; i < this->points.size(); i++)
			{
				if (this->points[i] != 0)
				{
					delete this->points[i];
					this->points[i] = 0;
				};
			};

			this->points.clear();
		};

		// Data Operations
		void insert(const Point3D* p)
		{
			this->points.push_back((Point3D*)p);
		};

		// Search Methods
		inline int64_t find(const Vector3f& position)
		{
			/*
				Given a 3D vector representing a position, find the first point in this 
				point table that is at that position and return its' index in the vector. 
				If no point is found, returns -1.
			*/

			for (int64_t i = 0; i < this->points.size(); i++)
			{
				if (this->points[i]->position == position)
				{
					return i;
				};
			};

			return -1;
		};

		// Access Methods
		inline Point3D* getPoint(const uint64_t& i)
		{
			/*
				Given an index to a point in this table, returns the associated pointer. If 
				the index is invalid, returns a null pointer (0).
			*/

			if (i < this->points.size())
			{
				return this->points[i];
			};

			return 0;
		};
		inline Point3D* getPoint(const Vector3f& position)
		{
			/*
				Given a 3D position, return a pointer to the first point found in the table 
				at that position. If no such point is found, returns a nullptr (0).
			*/

			int64_t i = this->find(position);
			if (i > 0)
			{
				return this->getPoint((uint64_t)i);
			};

			return 0;
		};

		// Serialization
		inline std::string toString() const
		{
			std::string str = "\n";
			for (auto point : points)
			{
				str += point->toString() + "\n";
			};
			str += "\n";
			return str;
		};
	};

	// Point Cloud Map Structure
	struct PointCloudMap
	{
		/*
			# Point Cloud Map (struct)

			## Description
			A map associating 3D points with type information.

			## Usage
			
			### Access Points in a PointCloudMap
			Note that this will return a pointer of type Point3D, which stores a 
			pcm::Vector3f and a reference id.
			```
				// Some arbitrary coordinates
				float x = 0.0f;
				float y = 1.0f;
				float z = -100.0f;

				// Get the point at those coordinates
				pmc::Point3D* p = yourPCM.getPoint({ x, y, z });
			```

			### Load a PCM from a String
			```
				std::string somePCMData = read_file("some/pcm/file.pcm");
				pcm::PointCloudMap myPCM;
				myPCM.fromString(somePCMData);
			```

			### Serialize a PCM to a String
			```
				std::string pcmString = yourPCM.toString();
			```
		*/

		/* Elements */
		ReferenceTable references;
		PointTable points;

		/* Methods */

		// Utility Methods
		inline void clear()
		{
			this->references.clear();
			this->points.clear();
		};

		// Operation Methods
		inline bool addReference(const ReferenceId& _id, const Reference& _ref)
		{
			return this->references.insert(_id, _ref);
		};
		inline bool addReference(const std::string& _id, const std::string& _type, const std::string& _data)
		{
			/*
				Adds a reference (id, type, value) to the point cloud map's internal 
				reference table and returns the result of the operation. If the given
				reference is already found in the table this will return false.
			*/

			ReferenceId id(_id);
			Reference ref(_type, _data);
			return this->addReference(id, ref);
		};

		inline bool addPoint(const ReferenceId& id, const Vector3f& pos)
		{
			/*
				Adds a point with an associated reference id to the point cloud map.
			*/

			// Create and add the point to the table,
			Point3D* point = new Point3D(id, pos);
			this->points.insert(point);
			return true;
		};
		inline bool addPoint(const std::string& id, const Vector3f& pos)
		{
			ReferenceId referenceId(id);
			return this->addPoint(referenceId, pos);
		};
	
		// removeReference(const ReferenceId& id);
		// removeReference(std::string id);
		
		// @todo: think through and design the functions listed below.
		// removePoint(std::string id, Vector3f pos);
		// removePoint(std::string id);
		// removeChunk(); // Remove all points associated with a chunk?
		// removeValue(); // Remove all points associated with a value?

		// Access Methods
		inline Point3D* getPoint(const uint64_t& i)
		{
			return this->points.getPoint(i);
		};
		inline Point3D* getPoint(const Vector3f& pos)
		{
			return this->points.getPoint(pos);
		};

		// Serialization
		inline std::string toString() const
		{
			std::string code = "# Point Cloud File\n";
			code += "# Version " + std::string(POINT_CLOUD_MAP_LIBRARY_VERSION) + "\n";
			code += this->references.toString();
			code += this->points.toString();
			return code;
		};

		// Deserialization
		inline bool fromString(const std::string& text)
		{
			/*
				## Description
				Loads data into a PointCloudMap from a string fitting the *.pcm syntax 
				specification.

				## Developer Notes
				Multiple errors can arise during deserialization. Right now they silently 
				happen and the line which caused them is omitted. Since this library isn't
				integrated with a logging library - I'm not entirely sure how to give good
				feedback on error. I'll have to look into that later and update this function
				with the changes then.

				In general, this right now can only return true. So maybe that should factor
				into it. I don't know - it's always hard providing good feedback with limited
				logging systems in a single-header lib without transplanting a log sys into
				every one.
			*/

			// Allocate some variables,
			Vector3f position;

			// For each line in the text,
			std::vector<std::string> lines = split_lines(text);
			for (std::string& line : lines)
			{
				// If the line isn't empty,
				if (line.size() != 0)
				{
					// Tokenize the line,
					std::vector<std::string> tokens = split_string(line, " ");

					// If there's at least one token,
					if (tokens.size() > 0)
					{
						// Let's save some cycles,
						const char* typeString = tokens[0].c_str();

						// If the line is a reference,
						if (strcmp(typeString, "ref") == 0)
						{
							// If the expected number of tokens was found,
							if (tokens.size() >= 4)
							{
								// Generate the reference data and add it to the point cloud,
								this->addReference(ReferenceId(tokens[1]), Reference(tokens[2], tokens[3]));
							};
						}
						// Else, if the line is a point,
						else if (strcmp(typeString, "point") == 0)
						{
							// If the expected number of tokens was found,
							if (tokens.size() >= 5)
							{
								// Generate the vector from the (x,y,z) tokens,
								position.zero();

								position.value[0] = atof(tokens[2].c_str());
								position.value[1] = atof(tokens[3].c_str());
								position.value[2] = atof(tokens[4].c_str());

								// Add the point with the given reference id,
								this->addPoint(tokens[1], position);
							};
						};
					};
				};
			};

			return true;
		};
	};
};
#endif
