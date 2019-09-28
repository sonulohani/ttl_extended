#include <windows.h>
#include "ttl/sccs/sccs.hpp"
#include <ios>

struct done {};

// all messages
typedef ttl::var::variant<
	int
	,done
> messages;

typedef ttl::sccs::agent< messages > agent;  //our agent type

// agent action
agent::message_pnt cons_input( agent::message_pnt )
{
	int n;
	std::cin>>n;
	if( std::cin.fail() )
	{
		return agent::message_pnt( new messages(done()) );
	}
	return agent::message_pnt( new messages(n) );
}

struct print_visitor
{
	void operator()( int d )
	{
		std::cout<<d<<"\n";
	}
	template< typename T >
	void operator()( const T& d )
	{
	}
};

// agent action
agent::message_pnt cons_output( agent::message_pnt m )
{
	print_visitor v;
	ttl::var::apply_visitor(v,*m);
	return agent::message_pnt();
}


void sccs_test()
{
	//console input agent
	agent::agent_pnt input_agent(
		ttl::sccs::start<agent>()  //start
		->act( cons_input, "console_input" ) //agent action - next input (int)
		->attach(
			// post the input
			agent::agent_pnt( 
				ttl::sccs::start<agent>()->out<int>()
			)
			// OR post 'done'
			+agent::agent_pnt(
				ttl::sccs::start<agent>()->out<done>()->stop()
			)
		)
	);
	input_agent->repeat(); //repeat indefinitely

	//console output agent
	agent::agent_pnt output_agent(
		// output the input
		agent::agent_pnt(
			ttl::sccs::start<agent>()
			->in<int>()  //wait for the next input
			->act( cons_output, "console_output" )  //agent action - print the input
		)
		// OR wait for 'done' and stop
		+agent::agent_pnt(
			ttl::sccs::start<agent>()
			->in<done>()->stop()
		)
	);
	output_agent->repeat(); //repeat indefinitely

	// the full agent is a product of input_agent and output_agent
	// it means that cons_in and cons_out will run concurently 
	// (in different threads)
	agent::agent_pnt full( input_agent * output_agent );

	// print the the full agent's equation
	full->print( std::cout );

	// run the agent
	ttl::sccs::runtime<agent> rt( full );

	// wait for the agent to finish
	rt.join();
}

