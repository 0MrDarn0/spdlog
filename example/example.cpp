//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
//
// spdlog usage example
//
//

#include "spdlog/spdlog.h"
#include "spdlog/logger.h"

spdlog::logger *get_logger();

int main(int, char *[])
{    
	int x = 4;
	spdlog::info("HELLO {}", "st at ic");
    spdlog::warn("HELLO {} *{} = {}", x, x, x*x);

    /*auto *l = get_logger();
    l->info("HEllo {}", "HG FS");*/
}