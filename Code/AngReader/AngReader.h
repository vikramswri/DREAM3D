///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ANGREADER_H_
#define ANGREADER_H_

#include <string>


/**
 * @brief Creates a "setter" method to set the property.
 */
#define angSetMacro(type, prpty) \
  void set##prpty(type value) { this->m_##prpty = value; }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define angGetMacro(type, prpty) \
  type get##prpty() { return m_##prpty; }

#define angInstanceProperty(type, prpty)\
  private:\
      type   m_##prpty;\
  public:\
    angSetMacro(type, prpty)\
    angGetMacro(type, prpty)

/**
 * @brief Creates a "setter" method to set the property.
 */
#define angSetStringMacro( prpty, varname) \
  void set##prpty(const std::string &value) { this->varname = value; }

/**
 * @brief Creates a "getter" method to retrieve the value of the property.
 */
#define angGetStringMacro( prpty, varname) \
  std::string get##prpty() { return varname; }

/**
 * @brief Creates setters and getters in the form of 'setXXX()' and 'getXXX()' methods
 */
#define angStringProperty(prpty)\
  private:\
      std::string   m_##prpty;\
  public:\
  angSetStringMacro(prpty, m_##prpty)\
  angGetStringMacro(prpty, m_##prpty)

#define STATIC_STRING_CONSTANT(str, quoted)\
  static std::string str##() { return std::string(quoted); }

/**
* @class AngReader AngReader.h AngReader/AngReader.h
* @brief This class is a self contained TSL OIM .ang file reader. It should only be dependent on
* standard C++ features
* @author Michael A. Jackson for BlueQuartz Software
* @date Mar 1, 2010
* @version 1.0
*/
class AngReader
{
  public:
    AngReader();
    virtual ~AngReader();

    /**
     * @brief Static method to use the correct method to deallocate the memory
     * for an array that was created in this class.
     * @param array The pointer to be deallocated
     * @return Should be a NULL pointer
     */
    static float* deleteArrayPointer(float* array);

    /** @brief Constants defined for the 5 orientation options */
    const static int UpperRightOrigin = 0;
    const static int UpperLeftOrigin = 1;
    const static int LowerLeftOrigin = 2;
    const static int LowerRightOrigin = 3;
    const static int NoOrientation = 4;

    /** @brief Allow the user to set the origin of the scan */
    angInstanceProperty(int, UserOrigin)

    /** @brief Sets the file name of the ang file to be read */
    angStringProperty(FileName)

    /** @brief Header Values from the TSL ang file */
    angInstanceProperty(float, TEMpixPerum)
    angInstanceProperty(float, XStar)
    angInstanceProperty(float, YStar)
    angInstanceProperty(float, ZStar)
    angInstanceProperty(float, WorkingDistance)
    angStringProperty(Phase)
    angStringProperty(MaterialName)
    angStringProperty(Formula)
    angStringProperty(Info)
    angStringProperty(Symmetry)
    angStringProperty(LatticeConstants)
    angStringProperty(NumberFamilies)
    angStringProperty(HKLFamilies)
    angStringProperty(Categories)
    angStringProperty(Grid)
    angInstanceProperty(float, XStep)
    angInstanceProperty(float, YStep)
    angInstanceProperty(float, ZStep)  // NOT actually in the file, but may be needed
    angInstanceProperty(float, ZPos)   // NOT actually in the file, but may be needed
    angInstanceProperty(float, ZMax)   // NOT actually in the file, but may be needed
    angInstanceProperty(int, NumOddCols)
    angInstanceProperty(int, NumEvenCols)
    angInstanceProperty(int, NumRows)
    angStringProperty(OIMOperator)
    angStringProperty(SampleID)
    angStringProperty(ScanID)


    /**
    * @brief Reads the complete TSL .ang file.
    * @return 1 on success
    */
    int readFile();

    /**
    * @brief Reads ONLY the header portion of the TSL .ang file
    * @return 1 on success
    */
    int readHeaderOnly();

    /**
     * @brief Determines if the memory that was allocated during the reading of
     * the file will be deallocated when this class goes out of scope or if the
     * programmer is responsible for cleaning up the memory with the 'delete'
     * command or other suitable command based on the method of allocation. Currently
     * there are 2 types of allocation based on how the library is compiled. Normal
     * malloc/free and SSE based malloc/free routines.
     */
    angInstanceProperty(bool, ManageMemory);

    /** @brief Get a pointer to the Phi1 data. Note that this array WILL be deleted
    * when this object is deleted. Most programs should copy the data from the array
    * into their own array storage. */
    float* getPhi1Pointer() { return m_Phi1; }
    float* getPhiPointer() { return m_Phi; }
    float* getPhi2Pointer() { return m_Phi2; }
    float* getXPosPointer() { return m_X; }
    float* getYPosPointer() { return m_Y; }
    float* getImageQualityPointer() { return m_Iq; }
    float* getConfidenceIndexPointer() { return m_Ci; }
    float* getPhasePointer() { return m_PhaseData; }


protected:

    /** @brief Allocates the proper amount of memory (after reading the header portion of the file)
    * and then splats '0' across all the bytes of the memory allocation
    */
    void initPointers( );

    /** @brief 'free's the allocated memory and sets the pointer to NULL
    */
    void deletePointers();

    /** @brief Parses the value from a single line of the header section of the TSL .ang file
    * @param line The line to parse
    */
    void parseHeaderLine(const std::string &line);

    /** @brief Parses the data from a line of data from the TSL .ang file
    * @param line The line of data to parse
    */
    void readData(const std::string &line);

private:
    float* m_Phi1;
    float* m_Phi;
    float* m_Phi2;
    float* m_Iq;
    float* m_Ci;
    float* m_PhaseData;
    float* m_X;
    float* m_Y;
    size_t m_NumberOfElements;

  /**
   * @brief Allocats a contiguous chunk of memory to store values from the .ang file
   * @param numberOfElements The number of elements in the Array. This method can
   * also optionally produce SSE aligned memory for use with SSE intrinsics
   * @return Pointer to allocated memory
   */
    template<typename T>
    T* allocateArray(size_t numberOfElements)
    {
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      T* m_buffer = static_cast<T*>( _mm_malloc (numberOfElements * sizeof(T), 16) );
#else
      T*  m_buffer = new T[numberOfElements];
#endif
      m_NumberOfElements = numberOfElements;
      return m_buffer;
    }

  /**
   * @brief Deallocates memory that has been previously allocated. This will set the
   * value of the pointer passed in as the argument to NULL.
   * @param ptr The pointer to be freed.
   */
    template<typename T>
    void deallocateArrayData(T* &ptr)
    {
      if (ptr != NULL && this->m_ManageMemory == true)
      {
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
        _mm_free(ptr );
#else
        delete[] ptr;
#endif
        ptr = NULL;
      }
    }

    AngReader(const AngReader&);    // Copy Constructor Not Implemented
    void operator=(const AngReader&);  // Operator '=' Not Implemented

};

#endif /* ANGREADER_H_ */
