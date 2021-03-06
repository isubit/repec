Research Papers in Economics (RePEc) Tutorial

This tutorial explains briefly the usage and principles for the RePEc module.
First of all, let's explain what RePEc stands for: Research Papers in
Economics, as the name implies, this is a platform to gather academical work in
 Economics and related. Let's suppose you need to research something in this
 area, a good starting point would be in a reliable website with a collectanea
 of more than 1.4 million works from 81 countries. But if you are on the other
 side and want to give more evidence to your academical work, you already have
 an institutional website (in Drupal, maybe), but a few people know about its
 existence. Where would you like to publish it? That's right, RePEc.

But there is an issue with RePEc, though. In order to publish your content in
their servers, you need to have one on your own (but if you already have a
website, this isn't quite an issue). Every day, RePEc checks its registered
servers for changes, case there is something different from what is already at
RePEc, it is updated. Why is that? Well, RePEc clusters the articles by
institution and that's one of the features that makes RePEc reliable. In order
words, there is a server for each institution registered at RePEc to publish
works produced by the staff of this institution. This is great, but the path
to see your article there is long and winding. To start, how can a regular
researcher in your institution publish his or her article? There may be a way
to feed the information of the article and submit the work through your
website and voilà, this work is already available through your website, all
RePEc needs to do is to find out the work was submitted. Nevertheless, to
maintain and update your work at RePEc, you need to follow a specific protocol,
defined through template files. There is a different template file for each of
your academical work with .rdf extension (all files with a different extension
is ignored by RePEc). A typical template file has a format like this:

Template-Type: ReDIF-Paper 1.0
Author-Name: Gary Painter
Author-Name: Stuart A. Gabriel
Author-Name: Dowell Myers
Title: Race, Immigrant Status, and Housing Tenure Choice
Abstract: This paper applies Census microdata from 1980 and 1990 to assess the
determinants of housing tenure choice among racial and ethnic groups in the Los
Angeles metropolitan area.  Like previous research, our results indicate that
endowment differences (income, education, and immigrant status) largely
explain the homeownership gap between Latinos and whites.  In contrast to
previous work, we find that Asians are as likely to choose homeownership as are
whites, and that status as an immigrant did not portend lower homeownership
rates among Asians.  However, the endowment-adjusted homeownership choice
differential between whites and blacks remains sizable; further, that gap more
than doubled between 1980 and 1990, to a full 11 percentage points.
Create-Date: 1999
File-URL: http://lusk.local.com/sites/default/files/working_papers/1999_109.pdf
File-Format: Application/pdf
Keywords: Housing Tenure Choice, Race, Immigrant Status, Homeownership
Number: 8660
Handle: RePEc:luk:wpaper:8660

Why do we need to follow this format? Well, that is how RePEc extracts
information from your server and updates its. From this example template, RePEc
understands that there are three authors: Gary Painter, Stuart Gabriel and
Dowell Myers; it also knows the title of the work and so on. Clearly you don't
want your users to write it themselves. First, it is pretty easy to write
something wrong. Second, after the second submission, your user will consider
if it is worthy the work to publish the work to RePEc. Third, your user
already fed this information in your website and he won't be happy to do the
same thing again. And the last, but not the least, they need to understand
Economics, not how things work behind the scenes.

Fortunately, if your website is in Drupal (if it isn't, what are you waiting
for?), now there is a module for this, to turn the info from the users into
these templates that not even you, my fellow reader, are interested to know.
This is what the RePEc module does. As it is a brand new module, it doesn't
offer all the desirable features, but it will in time. And if you want to
contribute to this module (thanks in advance), this article may be of your
interest. The core of the module is the settings page
(/admin/config/services/repec) as you can see in the image below.

The first step is to have a repository registered at 
<a href='http://repec.org/'>RePEC</a>. Every registered server is represented
by three letters, known as the archive code. In the example above, the archive
code is "luk", which represents USC Lusk Center for Real Estate institution.
The templates require the archive code in order to identify your institution.

You also have to inform the directory where the template files will be
generated. This is something you also have to inform to RePEc since that's
where its system will check for updates. For example, for our client the path
is http://lusk.usc.edu/repec/. Every day, RePEc goes there, compares the
existing files in this directory and updates its system. In the structure
bellow, we can see the structure that will be generated by the module and that
is understood by RePEc. First, we have the basic directory: RePEc, which is
informed to RePEc. Then, we have a directory with the archive code (the three
letters that represent your institution).

RePEc/
	luk/
		lukarch.rdf
		lukseri.rdf
		wpaper/
			rdf files for papers

In the luk folder, we have the first templates (remember the .rdf extension).
The main template is the [archive code]arch.rdf (in our example, lukarch.rdf)
and it contains basic information such as the institution, the name of the
provider, info about the maintainer, amongst others. We invite you to take a
look at these templates once they are generated by the module. This information
is necessary to be informed only once in the settings page. In the same
directory (RePEc/luk), the other template (lukseri.rdf) defines the series to
be used. A series is basically a set of similar templates, used to cluster the
works according to a criteria defined by the user. For example, the user may
cluster the articles according to the journal, or maybe by year, subject or
department. All the user needs to do is to create a different series and submit
the files accordingly. There are also five different types of templates: paper,
article, software, component, book and chapter.

The example above is a template of type paper (defined by the Template-Type
field). Each series is defined by six letters (wpaper in the example above).
For further information, feel free to
check http://ideas.repec.org/t/seritemplate.html. Here is the first
limitation of this module; one can only register one series only of the type
paper template. Future work will add the ability to register different series
of different or same type.

In the image below, you can see the info to be informed for the series. You
need to define the name and a code of six letters for the series (no need to
register it at RePEc, this info is stored in the [archive code]]seri.rdf file
and the module already does it for you!). You may also have a content type on
your own to match with the fields required for the template. In our case, we
have the Working Paper content type with the following fields: title, author,
year, tags, abstract, download. All content types require title and this is the
 title of your article. The other fields need to be manually matched to the
 fields required for the paper template, which are: Author-Name, File-URL,
 Keywords, Abstract and Create-Date.

What the module does is, whenever you save or edit a content of the informed
content type (again, in our example the working paper), a template is generated
for this content according to the association informed in the settings page.
For example, the Keywords field in the template is generated with the
information present in the Tags field of the content type. But be aware, once
you click on the submit button, all the directories and the files are
regenerated and replaced if already existed. The reason is to keep consistency
in case you change the archive code or an association. Finally, the Author-Name
field has the option to be split by the following delimiters: and & , ;. If you
 don't want that feature, just leave it unchecked.

And this is it. I hope you enjoy our brand new module to make your life, as
well as the lives of those in your institution, much easier!
