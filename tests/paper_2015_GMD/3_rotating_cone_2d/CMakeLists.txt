libmpdataxx_add_test_gi(rotating_cone_2d)
if(NOT USE_MPI)
  add_test(
    NAME rotating_cone_stats_diff 
    COMMAND bash -c "for i in stats*.txt; do 
      zdiff ${CMAKE_CURRENT_SOURCE_DIR}/refdata/$i.gz $i || exit 1; 
    done"
  )
endif()
