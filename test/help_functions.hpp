#include <string>
#include <vector>

#include <catch.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <graphtyper/graph/graph.hpp>
#include <graphtyper/utilities/system.hpp>


inline void
create_test_graph(std::string const & fasta,
                  std::string const & vcf,
                  std::string const & region,
                  bool const use_absolute_positions = true)
{
  gyper::GenomicRegion const genomic_region(region);
  std::stringstream vcf_path;
  vcf_path << gyper_SOURCE_DIRECTORY << vcf;
  std::stringstream reference_path;
  reference_path << gyper_SOURCE_DIRECTORY << fasta;

  BOOST_LOG_TRIVIAL(debug) << __HERE__ << " Creating a graph for " << reference_path.str() << " "
                           << vcf_path.str() << " at region " << region;

  gyper::construct_graph(reference_path.str(), vcf_path.str(), region, use_absolute_positions);
  std::stringstream graph_path;
  graph_path << gyper_SOURCE_DIRECTORY << "/test/data/graphs";

  std::string graph_directory(graph_path.str());

  // Check if directory exists, and of not, create it
  if (!gyper::is_directory(graph_directory))
    gyper::create_dir(graph_directory, 0755);

  graph_path << fasta.substr(20, fasta.size() - 3 - 20) << '_' << genomic_region.chr << ".grf";
  REQUIRE(gyper::graph.size() > 0);
  REQUIRE(gyper::graph.check());

  {
    std::ofstream ofs(graph_path.str().c_str(), std::ios::binary);
    REQUIRE(ofs.is_open());
    boost::archive::binary_oarchive oa(ofs);
    oa << gyper::graph;
  }

  // test open
  {
    gyper::Graph new_graph;
    std::ifstream ifs(graph_path.str().c_str(), std::ios::binary);
    REQUIRE(ifs.is_open());
    boost::archive::binary_iarchive ia(ifs);
    ia >> new_graph;

    REQUIRE(new_graph.size() == gyper::graph.size());
    REQUIRE(new_graph.genomic_region.chr == gyper::graph.genomic_region.chr);
    REQUIRE(new_graph.genomic_region.begin == gyper::graph.genomic_region.begin);
    REQUIRE(new_graph.genomic_region.end == gyper::graph.genomic_region.end);
  }
}
