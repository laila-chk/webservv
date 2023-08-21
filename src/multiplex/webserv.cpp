/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 20:24:24 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/05 14:29:27 by maamer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char **argv)
{
    Server http;
    Request request;
    try {
        if (argc > 2) {
            std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
            return EXIT_FAILURE;
        }
        http.init(argv[1]);
        http.run();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
