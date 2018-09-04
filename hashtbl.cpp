/*
    Andrew J Wood - COP4531
    Hash Analysis

    hashtbl.cpp

    Slave file for hashtbl.h
    Implementing the MaxBucketSize and Analysis methods for HashTable<K,D,H>

    Need to implement these files

*/
#include <ostream>

template <typename K, typename D, class H>
size_t HashTable<K,D,H>::MaxBucketSize () const
{
  size_t max = 0;

  typename fsu::Vector<BucketType>::ConstIterator i;

  //iterate through the bucketVector_ , determining which lists are not empty
  for (i = bucketVector_.Begin(); i != bucketVector_.End(); ++i)
  {
      if ((*i).Size() > max)
          max = (*i).Size();
  }

  return max;
}

template <typename K, typename D, class H>
void HashTable<K,D,H>::Analysis (std::ostream& os) const
{
  fsu::Vector<size_t> sizeCounter(MaxBucketSize() + 1, 0);

  // declare column widths
  int width0 = 10, width1 = 13, width2 = 15;
  int startheader = width0 - 4; // output ' ' at this width preceeding table

  //use existing method to get table size.
  size_t tableSize;
  tableSize = this->Size();

  size_t numBuckets;
  numBuckets = numBuckets_;

  //iterates through the vector of lists, determines if each list is empty.
  //If the list is not empty, increment the nonEmptyBuckets variable.
  size_t nonEmptyBuckets = 0;

  typename fsu::Vector<BucketType>::ConstIterator i;
  //iterate through the bucketVector_ , determing which lists are not empty
  for (i = bucketVector_.Begin(); i != bucketVector_.End(); ++i)
  {
      if (!(*i).Empty())
      {
        ++nonEmptyBuckets;
      }
      size_t bucketSize = (*i).Size();
      ++sizeCounter[bucketSize];
  }

  double loadFactor = 0;
  loadFactor = static_cast<double>(tableSize) / numBuckets;

  double expectedSearchTime = 0;
  expectedSearchTime = 1 + loadFactor;

  //Actual search time computation involves
  double actualSearchTime = 0;
  actualSearchTime = 1 + static_cast<double>(tableSize) / nonEmptyBuckets;


  os << "\n";
  os << std::setw(startheader) << ' ' << "table size:" << "           " << tableSize << "\n";
  os << std::setw(startheader) << ' ' << "number of buckets:" << "    " << numBuckets << "\n";
  os << std::setw(startheader) << ' ' << "nonempty buckets:" << "     " << nonEmptyBuckets << "\n";
  os << std::setw(startheader) << ' ' << "max bucket size:" << "      " << this->MaxBucketSize() << "\n";
  os << std::setw(startheader) << ' ' << "load factor:" << "          " << std::setprecision(2) << std::fixed << loadFactor << "\n";
  os << std::setw(startheader) << ' ' << "expected search time:" << " " << std::setprecision(2) << std::fixed << expectedSearchTime << "\n";
  os << std::setw(startheader) << ' ' << "actual search time:" << "   " << std::setprecision(2) << std::fixed << actualSearchTime << "\n";
  os << "\n";

  // details header
  os << std::setw(startheader) << ' ' << "bucket size distributions" << '\n'
     << std::setw(startheader) << ' ' << "-------------------------" << '\n';
  os << std::setw(width0) << "size"
     << std::setw(width1) << "actual"
     << std::setw(width2) << "theory" << " (uniform random distribution)\n";
  os << std::setw(width0) << "----"
     << std::setw(width1) << "------"
     << std::setw(width2) << "------" << '\n';


  bool maxActual      = false;
  bool maxTheoretical = false;
  size_t bucketCounter = 0;

  //generate the vector containing the expected bucket numbers with size = table size and initial values of 0.0
  fsu::Vector<double> expdCount(tableSize + 1, 0.0);

  double numBucketsDecimal = static_cast<double>(numBuckets);
  double tableSizeDecimal = static_cast<double>(tableSize);

  expdCount[0] = (numBucketsDecimal - 1) / numBucketsDecimal;
  expdCount[0] = pow(expdCount[0],(tableSizeDecimal - 1));
  expdCount[0] *= (numBucketsDecimal - 1);
  for (size_t k = 1; k < (1 + tableSize); ++k)
  {
    expdCount[k] = ((tableSizeDecimal - k + 1) / ((numBucketsDecimal - 1) * k)) * expdCount[k-1];
  }

  while ((!maxActual) || (!maxTheoretical))
  {
      size_t maxBucketSize = this->MaxBucketSize();

      os << std::setw(width0) << bucketCounter;

        if (bucketCounter < sizeCounter.Size())
        {
          os << std::setw(width1) << sizeCounter[bucketCounter];
        }
        else
        {
          os << std::setw(width1) << ' ';
        }

         os << std::setw(width2) << std::setprecision(1) << std::fixed << expdCount[bucketCounter] << "\n";

      //check to see if we're reached the stop point
      if (bucketCounter >= maxBucketSize)
      {
          maxActual = true;

          //check to see if the next theoretical value is < 0.05.  If it is, set the flag to true.
          if (expdCount[bucketCounter + 1] < 0.05)
          {
            maxTheoretical = true;
          }
      }

      //increment the bucket counter
      ++bucketCounter;
  }
  os << "\n";

} // Analysis
