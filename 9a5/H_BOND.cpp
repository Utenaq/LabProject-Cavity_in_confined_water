#include "amber_netcdf.hpp"
#include "amber_parm_1.0.hpp"
#include "vector_calc.h"
//~ #include <string.h>
//~ #include <fstream>
//~ #include <iostream>
//~ #include <stdlib.h>
//~ #include <stdio.h>


#define z_axis_modify  2
#define deviation_y_center 10
#define deviation_x_center 10
#define start_nc 12
#define end_nc  16
#define select_boundary 3.55
#define hbond_cutoff_length 3.5
#define hbond_cutoff_angle 135.0
#define name_parm7 "density_dis9a5.parm7" 
//~ #define name_nc "water_ion_graphene_10a5"

int main()
{
	typedef std::vector<double>::size_type index;
	std::cout << "program to calculate the average H-BOND number:angle135 distance9.5"<< "\n"<< std::endl;
	
	double total_hbond = 0;
	double total_wat =0;
	
	for(int nc = start_nc; nc  != end_nc+1; ++nc)
	{
        char name_nc[64];
        sprintf(name_nc, "density_dis9a5_%d.nc",nc);
		amber_parm parm_name(name_parm7);
        nctraj nc_data(name_nc);
        printf("nc_file = %d\n",nc);
        int total_frame = nc_data.frames_number();
		std::cout << "total frame: " << total_frame << std::endl;
	
	
	    std::vector<index> O_WAT_id = parm_name.id_by_type("OW");	
     
       for(int frame =0; frame != total_frame; ++frame)
       //~ for(int frame =0; frame != 100; ++frame)
       {
		   if((frame %100) == 0)
		   {
		   std::cout << "frame_now :            " << frame <<std::endl;
	       }
	     
	       double C_z_coor_sum_1 = 0, C_z_coor_sum_2 = 0;
           double C_z_coor_average_1, C_z_coor_average_2;	
	       double C_x_coor_sum = 0, C_y_coor_sum = 0;
           double C_x_coor_center, C_y_coor_center;        
           double Z_UP,Z_DOWN,Y_UP,Y_DOWN,X_UP,X_DOWN;	      
            
	        for( index C_index = 0 ; C_index != 1400 ; ++C_index)
	        {
		           C_z_coor_sum_1 += nc_data.atom_coordinate(frame, C_index)[2];
		           C_z_coor_sum_2 += nc_data.atom_coordinate(frame, (1400+C_index))[2];
		           C_y_coor_sum += nc_data.atom_coordinate(frame, C_index)[1];
   		           C_x_coor_sum += nc_data.atom_coordinate(frame, C_index)[0];		
	         }		    
	         C_z_coor_average_1 = C_z_coor_sum_1/1400;
	         C_z_coor_average_2 = C_z_coor_sum_2/1400;
	         C_y_coor_center = C_y_coor_sum/1400;
	         C_x_coor_center = C_x_coor_sum/1400;
	            
	         Z_UP = C_z_coor_average_2;
             Z_DOWN = C_z_coor_average_1;
             Y_UP = C_y_coor_center + deviation_y_center;
             Y_DOWN = C_y_coor_center - deviation_y_center;
             X_UP = C_x_coor_center + deviation_x_center ;
             X_DOWN = C_x_coor_center - deviation_x_center ;
	          
	         std::vector<index> O_WAT_IN_C_id, select_wat_id;       
	         std::vector<double> O_coor,O_coor_initial;
	
		      for(index i = 0; i != O_WAT_id.size(); ++i)
			  {
			      O_coor = nc_data.atom_coordinate(frame, O_WAT_id[i]);
				   if( O_coor[2] < Z_UP && O_coor[2] > Z_DOWN && O_coor[0] < X_UP && O_coor[0] > X_DOWN && O_coor[1] < Y_UP && O_coor[1] > Y_DOWN)
				   {
						 O_WAT_IN_C_id.push_back(O_WAT_id[i]);
					}
				   if( O_coor[2] < Z_UP && O_coor[2] > Z_DOWN && O_coor[0] < (X_UP+select_boundary) && O_coor[0] > (X_DOWN-select_boundary) && O_coor[1] < (Y_UP+select_boundary) && O_coor[1] > (Y_DOWN-select_boundary))
				   {
						 select_wat_id.push_back(O_WAT_id[i]);
					}					
		       }
		       
		       total_wat += O_WAT_IN_C_id.size();
		       
		       //~ std::cout << "size: " << O_WAT_IN_C_id.size() << " " << select_wat_id.size() << std::endl;
		      for(index i =0; i != O_WAT_IN_C_id.size(); ++i)
		      {
				  for(index j =0; j != select_wat_id.size(); ++j)
				  {
					  if( O_WAT_IN_C_id[i] != select_wat_id[j] )
					  {
						  double distance;
						  //~ std::cout << nc_data.atom_coordinate(frame,O_WAT_IN_C_id[i])[0] << "   " << nc_data.atom_coordinate(frame,select_wat_id[j])[0]<< std::endl;
						  distance   = sqrt(pow((nc_data.atom_coordinate(frame,O_WAT_IN_C_id[i])[0]- nc_data.atom_coordinate(frame,select_wat_id[j])[0]),2) +
						                         pow((nc_data.atom_coordinate(frame,O_WAT_IN_C_id[i])[1]- nc_data.atom_coordinate(frame,select_wat_id[j])[1]),2) +
						                         pow((nc_data.atom_coordinate(frame,O_WAT_IN_C_id[i])[2]- nc_data.atom_coordinate(frame,select_wat_id[j])[2]),2));	
						  //~ std::cout<< "distance_origion:   "<<distance<<std::endl;
						  if(distance < hbond_cutoff_length)
						  {
							  //~ std::cout << "distance: " << distance<<std::endl;
							  double angle;
							  std::vector<double> coor1,coor2,coor3,coor4;
							  coor1=vector_calc::vector_minus(nc_data.atom_coordinate(frame,O_WAT_IN_C_id[i]),nc_data.atom_coordinate(frame,select_wat_id[j]+1));
							  coor2=vector_calc::vector_minus(nc_data.atom_coordinate(frame,select_wat_id[j]),nc_data.atom_coordinate(frame,select_wat_id[j]+1));
							  coor3=vector_calc::vector_minus(nc_data.atom_coordinate(frame,O_WAT_IN_C_id[i]),nc_data.atom_coordinate(frame,select_wat_id[j]+2));
							  coor4=vector_calc::vector_minus(nc_data.atom_coordinate(frame,select_wat_id[j]),nc_data.atom_coordinate(frame,select_wat_id[j]+2));
							  angle = vector_calc::vector_angle(coor1,coor2);	
							  if(angle <  cos(hbond_cutoff_angle*vector_calc::PI/180.0))++total_hbond;       
							  //~ std::cout<< "angle1:  "<<angle<<std::endl;      
							  angle = vector_calc::vector_angle(coor3,coor4);	
						      if(angle <  cos(hbond_cutoff_angle*vector_calc::PI/180.0))++total_hbond;
						      //~ std::cout<<"angle2:  "<<angle<<std::endl;     
						  }
					  }
				  }
			  }

	     }       // frame end     
	}      // nc end
    //~ std::cout << total_hbond<<"  "<<total_wat<<std::endl;
    std::cout<< "average H-bond number:  "<< total_hbond*2/total_wat << std::endl;

	return 0;
}


